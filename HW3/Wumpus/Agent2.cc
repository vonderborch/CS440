// Agent.cc
// Christian Webber (11119437)
// CptS 440
// September 25th, 2014
// HW 2

#include <iostream>
#include "Agent2.h"

using namespace std;

Agent2::Agent2() { }

Agent2::~Agent2() { }

// initializes the agent at the beginning of each new try
void Agent2::Initialize()
{
	// reset basic variables
	av_b_wumpusdead = av_b_arrowused = av_b_hasgold = av_b_justshotarrow = false;
	av_af_directionfacing = AF_RIGHT;
	av_xy_position.x = av_xy_position.y = 0;
	av_xy_goal.x = av_xy_goal.y = 3;
	av_a_lastaction = CLIMB;
	av_i_tick = 0;

	// reset the agent's plan
	av_va_plan.clear();

	// reset the agent's world
	av_vat_world.clear();

	// open the debug file if we're in debug mode
	if (av_b_debugmode)
	{
		remove(av_s_debugfile);
		av_ofs_stream.open(av_s_debugfile, std::ofstream::out | std::ofstream::app);
	}
}

// Goal: get gold and return to surface (climb at 1,1)
// Points:	+1000 for leaving cave with the gold
//			-1000 for falling into a pit or being eaten by the wumpus
//			-10 for shooting the arrow
//			-1 for each action used.
//
// Actions:		FORWARD, TURNLEFT, TURNRIGHT, GRAB, SHOOT, CLIMB
// Percepts:	Stench: The wumpus is in a neighboring tile.
//				Breeze: A pit is in a neighboring tile
//				Glitter: The gold is in this tile
//				Bump: Agent has hit a wall
//				Scream: the wumpus is dead!

// gets fired each turn the ai can move.
Action Agent2::Process(Percept& percept)
{
	return AgentTick(percept); // return the action that the tick function decided on
}

// gets fired on a game over
void Agent2::GameOver(int score)
{
	if (av_b_debugmode)
		av_ofs_stream.close();
}


// agent control logic each tick (gets fired each tick)
Action Agent2::AgentTick(Percept& percept)
{
	DebugPrintTick();

	Action action = FORWARD;

	// create knowledge on agent's current tile
	bool makenewplan = CreateKnowledge(percept);
	if (makenewplan)
		DebugPrintWorld();

	// did we hit a wall?
	if (percept.Bump)
	{
		// do stuffs with lastaction
	}

	// did the agent just shoot?
	if (av_b_justshotarrow)
	{
		// did the wumpus die?
		if (percept.Scream)
			av_b_wumpusdead = true;

		// mark that we need to make a new plan and that the arrow was used.
		av_b_arrowused = true;
		av_b_justshotarrow = false;

		av_va_plan = CreatePlan();

		DebugPrintActionsPlan();

		// do the next action on the plan
		action = av_va_plan.front();
		av_va_plan.erase(av_va_plan.begin());

		UpdatePosition(action);
	}
	// are we at the gold?!?!?????!?!?!?!
	else if (percept.Glitter)
	{
		// GRAB THE GOLD....IT'S MINE, ALL MINE!!!!!!!
		action = GRAB;
		av_b_hasgold = true;
		av_xy_goal.x = av_xy_goal.y = 0;
		av_va_plan = CreatePlan();

		av_ofs_stream << "Action Override: GRAB." << endl;
	}
	// are we by the evil evil wumpus that must die?
	else if (!av_b_wumpusdead && percept.Stench)
	{
		av_b_justshotarrow = true;
		action = SHOOT;
		av_ofs_stream << "Action Override: SHOOT." << endl;
	}
	else if (!av_b_wumpusdead && av_b_arrowused && percept.Stench)
	{
		av_va_plan = CreatePlan();

		DebugPrintActionsPlan();

		// do the next action on the plan
		action = av_va_plan.front();
		av_va_plan.erase(av_va_plan.begin());

		UpdatePosition(action);
	}
	else if (av_b_hasgold && av_xy_position.x == 0 && av_xy_position.y == 0)
	{
		action = CLIMB;
		av_ofs_stream << "Action Override: CLIMB." << endl;
	}
	// if not ( :( ), continue our regularly scheduled plan
	else
	{
		// create new plan (if necessary)
		if (makenewplan)
			av_va_plan = CreatePlan();

		DebugPrintActionsPlan();

		// do the next action on the plan
		action = av_va_plan.front();
		av_va_plan.erase(av_va_plan.begin());

		UpdatePosition(action);
	}

	// save that we did this action.
	av_a_lastaction = action;

	av_i_tick++;
	// return the action
	return action;
}

void Agent2::UpdatePosition(Action action)
{
	// are we turning left?
	if (action == TURNLEFT)
	{
		if (av_af_directionfacing == AF_UP)
			av_af_directionfacing = AF_LEFT;
		else if (av_af_directionfacing == AF_LEFT)
			av_af_directionfacing = AF_DOWN;
		else if (av_af_directionfacing == AF_DOWN)
			av_af_directionfacing = AF_RIGHT;
		else if (av_af_directionfacing == AF_RIGHT)
			av_af_directionfacing = AF_UP;
	}
	// are we turning right?
	else if (action == TURNRIGHT)
	{
		if (av_af_directionfacing == AF_UP)
			av_af_directionfacing = AF_RIGHT;
		else if (av_af_directionfacing == AF_RIGHT)
			av_af_directionfacing = AF_DOWN;
		else if (av_af_directionfacing == AF_DOWN)
			av_af_directionfacing = AF_LEFT;
		else if (av_af_directionfacing == AF_LEFT)
			av_af_directionfacing = AF_UP;
	}
	// are we going forward?
	else if (action == FORWARD)
	{
		if (av_af_directionfacing == AF_UP)
			av_xy_position.y++;
		else if (av_af_directionfacing == AF_LEFT)
			av_xy_position.x--;
		else if (av_af_directionfacing == AF_DOWN)
			av_xy_position.y--;
		else if (av_af_directionfacing == AF_RIGHT)
			av_xy_position.x++;
	}
}

// updates the agent's knowledge of the world
bool Agent2::CreateKnowledge(Percept& percept)
{
	// are we creating knowledge for the first time?
	if (av_vat_world.size() == 0)
	{
		vector<AgentTile> temp_vat;
		AgentTile home;
		home.knowledge.push_back(AK_SAFE);
		if (percept.Breeze)
			home.knowledge.push_back(AK_BREEZE);
		if (percept.Stench)
			home.knowledge.push_back(AK_STENCH);
		temp_vat.push_back(home);

		av_vat_world.push_back(temp_vat);
		return true;
	}
	// otherwise...
	else
	{
		// get x and y sizes
		int worldx = av_vat_world.size(),
			worldy = av_vat_world[0].size();

		// do we need to increase the x size of the world?
		if (worldx <= av_xy_position.x)
		{
			vector<AgentTile> temp_vat;
			AgentTile temp;
			temp.knowledge.push_back(AK_UNKNOWN);
			// resize the x grid with a new column of unknown y's
			temp_vat.resize(worldy, temp);
			av_vat_world.push_back(temp_vat);
		}
		// do we need to increase the y size of the world?
		if (worldy <= av_xy_position.y)
		{
			AgentTile temp;
			temp.knowledge.push_back(AK_UNKNOWN);
			// resize each x coord for the new y's and fill them with unknown tiles
			for (int i = 0; i < worldx; i++)
				av_vat_world[i].resize(av_xy_position.y + 1, temp);
		}

		// fill in knowledge about the current coords if we haven't already
		if (av_vat_world[av_xy_position.x][av_xy_position.y].knowledge[0] == AK_UNKNOWN)
		{
			bool update = false;
			AgentTile temp;
			temp.knowledge.push_back(AK_SAFE);
			if (percept.Breeze)
			{
				temp.knowledge.push_back(AK_BREEZE);
				update = true;
			}
			if (percept.Stench)
			{
				temp.knowledge.push_back(AK_STENCH);
				update = true;
			}

			// push the new knowledge to the agent's memory
			av_vat_world[av_xy_position.x][av_xy_position.y] = temp;

			// return that whether we need to update the plan (if there is a breeze or stench)
			return update;
		}
	}

	// default to return false
	return false;
}

// creates a plan of actions based on the agent's knowledge of the world.
vector<Action> Agent2::CreatePlan()
{
	// return the Plan.
	return CreatePlanActions(CreatePlanSearch());
}

// Searches for the best route to take, factoring in known and unknown coordinates.
// Uses A*, f(n) = g(n) + h(n) = g(n) + (dist_from_goal^3 + (stenchweight*isstench*iswumpusalive) + (breezeweight*isbreeze) + (unknownweight*isunknown) - (safeweight * issafe))
//			f(n) >= 0
//			Tries actions in order: F, L, R
vector<AgentCoords> Agent2::CreatePlanSearch()
{
	vector<AgentCoords> coords, empty;
	// setup the root node...
	AgentSearchNode root;
	root.SetCoords(av_xy_position);
	root.SetGoal(av_xy_goal);
	root.SetWorld(av_vat_world);
	root.SetWumpusDead(av_b_wumpusdead);
	root.SetDirection(av_af_directionfacing);
	root.SetCost(root.CalculateCost(av_xy_position, av_xy_position));

	// setup vectors
	vector<AgentSearchNode> setclosed,
							setopen,
							camefrom;
	setopen.push_back(root);

	while (setopen.size() > 0)
	{
		// Assign Current node to the node with the best (lowest) value
		AgentSearchNode current;
		int i = 0;
		for each (AgentSearchNode node in setopen)
		{
			if (node.GetCost() < current.GetCost())
				if (!CreatePlanSearchInSet(node, setclosed))
					current = node;
			i++;
		}

		// add current to camefrom
		camefrom.push_back(current);

		// check if we're at the goal yet
		if (current.IsGoal())
			return CreatePlanSearchReconstructPath(camefrom, current);

		// remove current node from the open set
		setopen.erase(setopen.begin() + i - 1);
		// add current node to the closed set
		setclosed.push_back(current);

		// add the neighbors to the openset
		vector<AgentSearchNode> neighbors = current.GetSuccessors();
		for each (AgentSearchNode node in neighbors)
		{
			// has the neighbor node already been visited?
			if (CreatePlanSearchInSet(node, setclosed))
				continue;
			// has the neighbor already been added to the openset?
			if (!CreatePlanSearchInSet(node, setopen))
				setopen.push_back(node);
		}
	}

	return empty;
}

vector<AgentCoords> Agent2::CreatePlanSearchReconstructPath(vector<AgentSearchNode> camefrom, AgentSearchNode current)
{
	vector<AgentCoords> coordslist;
	if (CreatePlanSearchInSet(current, camefrom))
	{
		current = CreatePlanSearchGetCurrentParentInfo(current, camefrom);
		AgentSearchNode parent;
		parent.SetCoords(current.GetParentCoords());
		parent.SetDirection(current.GetParentDirection());

		vector<AgentCoords> temp = CreatePlanSearchReconstructPath(camefrom, parent);

		for each (AgentCoords node in temp)
			coordslist.push_back(node);

		coordslist.push_back(current.GetCoords());
		return coordslist;
	}
	else
	{
		if (current.GetCoords().x != -1 && current.GetCoords().y != -1)
			coordslist.push_back(current.GetCoords());
		return coordslist;
	}
}

// Fill in parent info of the current node, assumes CreatePlanSearchInSet is run first and returns true
AgentSearchNode Agent2::CreatePlanSearchGetCurrentParentInfo(AgentSearchNode node, vector<AgentSearchNode> set)
{
	if (node.GetCoords().x != -1 && node.GetCoords().y != -1)
	{
		if (set.size() > 0)
		{
			// for each node in the set...
			for each (AgentSearchNode setnode in set)
			{
				// are the nodes the same?
				if (node.GetCoords().x == setnode.GetCoords().x)
					if (node.GetCoords().y == setnode.GetCoords().y)
						if (node.GetDirection() == setnode.GetDirection())
						{
							node.SetParentCoords(setnode.GetParentCoords());
							node.SetParentDirection(setnode.GetParentDirection());
						}
			}
		}
	}
	return node;
}

// searches a set of nodes for any matches to the node.
bool Agent2::CreatePlanSearchInSet(AgentSearchNode node, vector<AgentSearchNode> set)
{
	if (node.GetCoords().x != -1 && node.GetCoords().y != -1)
	{
		if (set.size() > 0)
		{
			// for each node in the set...
			for each (AgentSearchNode setnode in set)
			{
				// are the nodes the same?
				if (node.GetCoords().x == setnode.GetCoords().x)
					if (node.GetCoords().y == setnode.GetCoords().y)
						if (node.GetDirection() == setnode.GetDirection())
							return true;
			}
		}
	}

	// if none are the same, return false
	return false;
}

// Creates a list of actions to follow based on the list of coordinates passed to it.
vector<Action> Agent2::CreatePlanActions(vector<AgentCoords> coordplan)
{
	vector<Action> actions; // blank list of vectors
	AgentCoords lastcoord = av_xy_position;
	AgentFacing lastfacing = av_af_directionfacing;
	vector<AgentCoords> uniqueplan;

	// clean up coordplan
	for each (AgentCoords coord in coordplan)
	{
		bool shouldadd = true;
		for each(AgentCoords ucoord in uniqueplan)
		{
			if (coord.x == ucoord.x && coord.y == ucoord.y)
			{
				shouldadd = false;
				break;
			}
		}
		if (shouldadd)
		{
			uniqueplan.push_back(coord);
		}
	}

	DebugPrintCoordPlan(uniqueplan);

	// foreach coordinate in the list...
	for each (AgentCoords coord in uniqueplan)
	{
		// ignore the current positions coord, we're already there and need no actions to get to it!
		if (coord.x != av_xy_position.x || coord.y != av_xy_position.y)
		{
			int cx = coord.x, cy = coord.y,
				lx = lastcoord.x, ly = lastcoord.y;
			// going left?
			if (cx < lx)
			{
				// if we're facing the wrong direction, we need to turn before we can go forward!
				if (lastfacing == AF_DOWN)
				{
					actions.push_back(TURNRIGHT);
				}
				else if (lastfacing == AF_UP)
				{
					actions.push_back(TURNLEFT);
				}
				else if (lastfacing == AF_RIGHT)
				{
					actions.push_back(TURNRIGHT);
					actions.push_back(TURNRIGHT);
				}

				actions.push_back(FORWARD);
				lastfacing = AF_LEFT;
			}
			// going right?
			if (cx > lx)
			{
				// if we're facing the wrong direction, we need to turn before we can go forward!
				if (lastfacing == AF_DOWN)
				{
					actions.push_back(TURNLEFT);
				}
				else if (lastfacing == AF_UP)
				{
					actions.push_back(TURNRIGHT);
				}
				else if (lastfacing == AF_LEFT)
				{
					actions.push_back(TURNRIGHT);
					actions.push_back(TURNRIGHT);
				}

				actions.push_back(FORWARD);
				lastfacing = AF_RIGHT;
			}
			// going down?
			if (cy < ly)
			{
				// if we're facing the wrong direction, we need to turn before we can go forward!
				if (lastfacing == AF_LEFT)
				{
					actions.push_back(TURNLEFT);
				}
				else if (lastfacing == AF_RIGHT)
				{
					actions.push_back(TURNRIGHT);
				}
				else if (lastfacing == AF_UP)
				{
					actions.push_back(TURNRIGHT);
					actions.push_back(TURNRIGHT);
				}

				actions.push_back(FORWARD);
				lastfacing = AF_DOWN;
			}
			// going up?
			if (cy > ly)
			{
				// if we're facing the wrong direction, we need to turn before we can go forward!
				if (lastfacing == AF_LEFT)
				{
					actions.push_back(TURNRIGHT);
				}
				else if (lastfacing == AF_RIGHT)
				{
					actions.push_back(TURNLEFT);
				}
				else if (lastfacing == AF_DOWN)
				{
					actions.push_back(TURNRIGHT);
					actions.push_back(TURNRIGHT);
				}

				actions.push_back(FORWARD);
				lastfacing = AF_UP;
			}
		}

		// update lastcoord with the current coord!
		lastcoord = coord;
	}

	return actions;
}

void Agent2::DebugPrintTick()
{
	if (av_b_debugmode)
		av_ofs_stream << endl << endl << "WEBBERAGENT TICK #" << av_i_tick << endl << endl;
}

void Agent2::DebugPrintWorld()
{
	if (av_b_debugmode)
	{
		av_ofs_stream << "World Knowledge List:" << endl;
		av_ofs_stream << endl << "End World Knowledge" << endl;
	}
}

void Agent2::DebugPrintCoordPlan(vector<AgentCoords> coordslist)
{
	if (av_b_debugmode)
	{
		av_ofs_stream << "List of Coordinates to Visit:" << endl;
		for each (AgentCoords coord in coordslist)
		{
			av_ofs_stream << "(" << coord.x << ", " << coord.y << ") ";
		}
		av_ofs_stream << endl << "End List of Coordinates to Visit" << endl;
	}
}

void Agent2::DebugPrintActionsPlan()
{
	if (av_b_debugmode)
	{
		av_ofs_stream << "List of Actions:" << endl;
		for each (Action action in av_va_plan)
		{
			av_ofs_stream << "(";
			if (action == FORWARD)
				av_ofs_stream << "Forward";
			else if (action == TURNLEFT)
				av_ofs_stream << "TurnLeft";
			else if (action == TURNRIGHT)
				av_ofs_stream << "TurnRight";

			av_ofs_stream << ") ";
		}
		av_ofs_stream << endl << "End List of Actions" << endl;
	}
}