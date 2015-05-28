// Agent.cc
// Christian Webber (11119437)
// CptS 440
// September 10th, 2014
// HW 1

#include <iostream>
#include "Agent3.h"

using namespace std;

// initialize the agent for this map!
Agent3::Agent3()
{
	// world size guesstimate
	agent_knowledge.worldSize = 4; // true in hw3

	// setup world information variables
	ResizeWorld(agent_knowledge.worldSize, agent_knowledge.worldSize);

	home = Location(1, 1);
	gold = Location(4, 4);
}

// clean out the agent
Agent3::~Agent3() { }

// initialize the agent for this next try
void Agent3::Initialize()
{
	// agent knowledge information
	agent_knowledge.agentLocation = Location(1, 1);
	agent_knowledge.agentOrientation = RIGHT;
	agent_knowledge.agentHasArrow = true;
	agent_knowledge.agentHasGold = false;
	agent_knowledge.wumpusAlive = true;
	agent_knowledge.agentAlive = true;
	agent_knowledge.agentInCave = true;

	// Setup Agent plan variable
	agent_plan.clear();
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
Action Agent3::Process(Percept& percept)
{
	// did we detect a bump? if we did reset agent knowledge to that of last tick
	if (percept.Bump)
		agent_knowledge = agent_knowledge_last;

	// get the action that the agent has to do this turn...
	Action action = Update(percept);

	// update the agent's knowledge about itself and the world...
	if (action == FORWARD)
	{
		if (agent_knowledge.agentOrientation == UP)
			agent_knowledge.agentLocation.Y++;
		else if (agent_knowledge.agentOrientation == DOWN)
			agent_knowledge.agentLocation.Y--;
		else if (agent_knowledge.agentOrientation == LEFT)
			agent_knowledge.agentLocation.X--;
		else if (agent_knowledge.agentOrientation == RIGHT)
			agent_knowledge.agentLocation.X++;
	}
	else if (action == TURNLEFT)
	{
		if (agent_knowledge.agentOrientation == UP)
			agent_knowledge.agentOrientation = LEFT;
		else if (agent_knowledge.agentOrientation == DOWN)
			agent_knowledge.agentOrientation = RIGHT;
		else if (agent_knowledge.agentOrientation == LEFT)
			agent_knowledge.agentOrientation = DOWN;
		else if (agent_knowledge.agentOrientation == RIGHT)
			agent_knowledge.agentOrientation = UP;
	}
	else if (action == TURNRIGHT)
	{
		if (agent_knowledge.agentOrientation == UP)
			agent_knowledge.agentOrientation = RIGHT;
		else if (agent_knowledge.agentOrientation == DOWN)
			agent_knowledge.agentOrientation = LEFT;
		else if (agent_knowledge.agentOrientation == LEFT)
			agent_knowledge.agentOrientation = UP;
		else if (agent_knowledge.agentOrientation == RIGHT)
			agent_knowledge.agentOrientation = DOWN;
	}
	else if (action == GRAB)
		agent_knowledge.agentHasGold = true;
	else if (action == SHOOT)
		agent_knowledge.agentHasArrow = false;

	// update last tick agent knowledge
	agent_knowledge_last = agent_knowledge;

	return action;
}

// agent's either died or succeeded
void Agent3::GameOver(int score) { }

// Resize the agent's world knowledge database.
void Agent3::ResizeWorld(int columns, int rows)
{
	agent_world.resize(columns, vector<WorldInfo>(rows));
}

// agent update tick
Action Agent3::Update(Percept& percept)
{
	// action variable!
	Action action = FORWARD;

	// update the world!
	bool update = UpdateWorld(percept);

	// update the plan!
	if (update)
	{
		if (agent_knowledge.agentHasGold)
			UpdatePlan(home, percept);
		else
			UpdatePlan(gold, percept);
	}

	// if the plan is empty, default to go home.
	if (agent_plan.empty())
	{
		UpdatePlan(home, percept);
		agent_plan.push_back(CLIMB);
	}

	// if the plan is still empty, try climbing. otherwise pop off the front of the plan
	if (agent_plan.empty())
		action = CLIMB;
	else
	{
		action = agent_plan.front();
		agent_plan.pop_front();
	}

	// return the selected action!
	return action;
}

// Update the agent's world. return true if we need to update the plan (any percepts or logic)
bool Agent3::UpdateWorld(Percept& percept)
{
	bool update = false;
	//// WORLD KNOWLEDGE ARRAY SIZE CHECK ////
	int world_size_x = agent_world.size(), world_size_y = 0;
	if (world_size_x > 0)
		world_size_y = agent_world[0].size();
	// check if the world x size is still big enough. if not, resize it
	if (world_size_x == agent_knowledge.agentLocation.X)
		ResizeWorld(agent_knowledge.agentLocation.X, world_size_y);
	// check if the world y size is still big enough. if not, resize it
	if (world_size_y == agent_knowledge.agentLocation.Y)
		ResizeWorld(world_size_x, agent_knowledge.agentLocation.Y);

	//// VISIT THE LOCATION ////
	agent_world[agent_knowledge.agentLocation.X - 1][agent_knowledge.agentLocation.Y - 1].Visit(percept);
	if (percept.Breeze)
		update = true;
	else if (percept.Stench)
		update = true;
	else
		update = true;

	//// LOGIC ////
	// try finding a wumpus or a pit

	return update;
}

// update the agent's plan
void Agent3::UpdatePlan(Location destination, Percept& percept)
{
	// clear the current plan...
	agent_plan.clear();

	// first lets check if we've found the gold...
	if (percept.Glitter)
	{
		agent_plan.push_front(GRAB);
		PlanRoute(home, EO_NONE);
	}
	// now let's check if we've determined the wumpus location if it's still alive
	else if (agent_knowledge.wumpusAlive)
	{
		for (int x = 0; x < agent_world.size(); x++)
		{
			for (int y = 0; y < agent_world[x].size(); y++)
			{
				if (agent_world[x][y]._wumpus)
				{
					// get location and destination to shoot the wumpus from...
					Location wumpus = Location(x, y);
					Location dest = GetNearestHorizontalVerticalLocation(wumpus);
					EnhancedOrientation orient = EO_NONE;
					if (dest.X < wumpus.X)
						orient = EO_RIGHT;
					else if (dest.X > wumpus.X)
						orient = EO_LEFT;
					else if (dest.Y < wumpus.Y)
						orient = EO_UP;
					else if (dest.Y > wumpus.Y)
						orient = EO_DOWN;

					PlanRoute(dest, orient);
					agent_plan.push_back(SHOOT);
					PlanRoute(gold, EO_NONE);
					return;
				}
			}
		}

		if (agent_knowledge.agentHasGold)
			PlanRoute(home, EO_NONE);
		else
			PlanRoute(gold, EO_NONE);
	}
}

// get the nearest horizontal or vertical location for the player along the destination x/y
Location Agent3::GetNearestHorizontalVerticalLocation(Location destination)
{
	// assign output to current location
	Location output = agent_knowledge.agentLocation;

	// calc differences
	int xdiff = output.X - destination.X,
		ydiff = output.Y - destination.Y,
		xdiffabs = xdiff * xdiff,
		ydiffabs = ydiff * ydiff;

	// if either is 0, we don't need to move
	if (xdiff == 0 || ydiff == 0)
		return output;

	// go to a horizontal
	if (xdiffabs < ydiffabs)
		output.X -= xdiff;
	// go to a vertical
	else
		output.Y -= ydiff;

	// return output destination
	return output;
}

// plan a route to the destination...
bool Agent3::PlanRoute(Location destination, EnhancedOrientation orient)
{
	vector<SearchNode*> frontier,
					    explored;
	SearchNode* initial = new SearchNode(agent_knowledge.agentLocation, destination, agent_knowledge.agentOrientation, FORWARD, agent_world, NULL),
				current, next;

	if (true)
	{

	}

	// if we're already at the destination, no need to go back...
	if (initial->location == destination && orient == EO_NONE)
		return true;

	// push in current node...
	if (LocationsNotEqual(initial->location, destination))
	{
		frontier.push_back(initial);

		// while we still have nodes to look at...
		while (!frontier.empty())
		{
			// get the cheapest node to explore...
			current = *frontier.front();
			int index = 0;
			for (int i = 0; i < frontier.size(); i++)
			{
				if (current.cost > frontier[i]->cost)
				{
					current = *frontier[i];
					index = i;
				}
			}
			frontier.erase(frontier.begin() + index);
			explored.push_back(&current);

			if (current.location == destination)
				break;

			// get successors...
			vector<SearchNode> neighbors = current.GetSuccessors();
			for (int i = 0; i < neighbors.size(); i++)
			{
				// have we already explored this neighbor?
				if (NodeExistsIn(neighbors[i], explored))
					continue;
				// does the neighbor already exist in the frontier?
				if (!NodeExistsIn(neighbors[i], frontier))
					frontier.push_back(new SearchNode(neighbors[i]));

				if (true);
			}
		}
	}
	
	// do we need to be facing a specific way at out destination?
	if (orient != EO_NONE)
	{
		Orientation agorient = agent_knowledge.agentOrientation;
		if (orient == EO_DOWN)
		{
			if (agorient == UP)
			{
				agent_plan.push_front(TURNLEFT);
				agent_plan.push_front(TURNLEFT);
			}
			else if (agorient == LEFT)
				agent_plan.push_front(TURNLEFT);
			else if (agorient == RIGHT)
				agent_plan.push_front(TURNRIGHT);
		}
		else if (orient == EO_LEFT)
		{
			if (agorient == RIGHT)
			{
				agent_plan.push_front(TURNLEFT);
				agent_plan.push_front(TURNLEFT);
			}
			else if (agorient == UP)
				agent_plan.push_front(TURNLEFT);
			else if (agorient == DOWN)
				agent_plan.push_front(TURNRIGHT);
		}
		else if (orient == EO_RIGHT)
		{
			if (agorient == LEFT)
			{
				agent_plan.push_front(TURNLEFT);
				agent_plan.push_front(TURNLEFT);
			}
			else if (agorient == DOWN)
				agent_plan.push_front(TURNLEFT);
			else if (agorient == UP)
				agent_plan.push_front(TURNRIGHT);
		}
		else
		{
			if (agorient == DOWN)
			{
				agent_plan.push_front(TURNLEFT);
				agent_plan.push_front(TURNLEFT);
			}
			else if (agorient == RIGHT)
				agent_plan.push_front(TURNLEFT);
			else if (agorient == LEFT)
				agent_plan.push_front(TURNRIGHT);
		}
	}

	// now that we've got current set to the destination (or we've failed to find anything...)
	if (&current == NULL)
		return false;
	else
	{
		// generate the plan!
		while (current != *initial)
		{
			agent_plan.push_front(current.action);
			current = *current.parent;
		}
		return true;
	}
}

bool Agent3::NodeExistsIn(SearchNode node, vector<SearchNode*> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (*list[i] == node)
			return true;
	}

	return false;
}