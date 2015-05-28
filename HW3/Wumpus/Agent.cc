// Agent.cc
// Christian Webber
// CptS 440
// HW 3

#include <iostream>
#include <algorithm>
#include "Agent.h"

using namespace std;


// ----- SearchState class -----

SearchState::SearchState (Location& loc, Orientation orient, Action act, SearchState* par)
{
	location = loc;
	orientation = orient;
	action = act;
	parent = par;
}

bool SearchState::operator== (const SearchState& searchState)
{
	if ((location == searchState.location) && (orientation == searchState.orientation))
	{
		return true;
	}
	return false;
}


// ----- Agent class -----

Agent::Agent()
{
	// nothing for now
}

Agent::~Agent()
{
	// nothing for now
}

void Agent::GameOver(int score)
{
	// nothing for now
}

void Agent::Initialize()
{
	worldState.worldSize = 4; // Given in HW2 problem definition
	worldState.agentLocation = Location(1,1);
	worldState.agentOrientation = RIGHT;
	worldState.agentHasArrow = true;
	worldState.agentHasGold = false;
	worldState.wumpusAlive = true;
	// Initialize safe and visited locations
	for (int x = 0; x <= 4; x++)
	{
		for (int y = 0; y <= 4; y++)
		{
			safeLocations[x][y] = false;
			visitedLocations[x][y] = false;
			stenchLocations[x][y] = false;
		}
	}
	safeLocations[1][1] = true;
	visitedLocations[1][1] = true;
	plan.clear();
}

Action Agent::Process(Percept& percept)
{
	Action action;
	Orientation orient;
	int x, y;

	UpdateState (percept);
	orient = worldState.agentOrientation;
	x = worldState.agentLocation.X;
	y = worldState.agentLocation.Y;
	Location home = Location(1,1);

	// Agent should always GRAB the gold and then leave cave
	if (percept.Glitter)
	{
		worldState.agentHasGold = true;
		plan.clear();
		plan.push_back(GRAB);
		PlanSafeRoute(home); // route home always exists
		plan.push_back(CLIMB);
	} else if ((percept.Stench) && (worldState.agentHasArrow)) {
		// check nearby locations to see if we've seen any other stench...
		int left = worldState.agentLocation.X - 1,
			right = worldState.agentLocation.X + 1,
			up = worldState.agentLocation.Y + 1,
			down = worldState.agentLocation.Y - 1,
			left2 = left - 1,
			right2 = right + 1,
			up2 = up + 1,
			down2 = down - 1;

		Direction directionfound = D_NONE;

		// have we detected another stench?
		// across left
		if (stenchLocations[left2][y])
		{
			directionfound = D_LEFT;
			safeLocations[left2][y] = true;
		}
		// across right
		else if (stenchLocations[right2][y])
		{
			directionfound = D_RIGHT;
			safeLocations[right2][y] = true;
		}
		// across up
		else if (stenchLocations[x][up2])
		{
			directionfound = D_UP;
			safeLocations[x][up2] = true;
		}
		// across down
		else if (stenchLocations[x][down2])
		{
			directionfound = D_DOWN;
			safeLocations[x][down2] = true;
		}
		// diagnol left up
		else if (stenchLocations[left][up])
		{
			directionfound = D_LEFTUP;
			safeLocations[left][up] = true;
		}
		// diagnol left down
		else if (stenchLocations[left][down])
		{
			directionfound = D_LEFTDOWN;
			safeLocations[left][down] = true;
		}
		// diagnol right up
		else if (stenchLocations[right][up])
		{
			directionfound = D_RIGHTUP;
			safeLocations[right][up] = true;
		}
		// diagnol right up
		else if (stenchLocations[right][down])
		{
			directionfound = D_RIGHTDOWN;
			safeLocations[right][down] = true;
		}

		// if we've found another stench in any of the directions
		if (directionfound != D_NONE)
		{
			// clear the plan if we've found another stench...
			plan.clear();
			// orient ourselfs if we need to...
			if (directionfound == D_LEFT || directionfound == D_LEFTDOWN || directionfound == D_LEFTUP)
			{
				// orient left
				if (orient == DOWN)
					plan.push_back(TURNRIGHT);
				else if (orient == UP)
					plan.push_back(TURNLEFT);
				else if (orient == RIGHT)
				{
					plan.push_back(TURNLEFT);
					plan.push_back(TURNLEFT);
				}
			}
			else if (directionfound == D_RIGHT || directionfound == D_RIGHTDOWN || directionfound == D_RIGHTUP)
			{
				// orient right
				if (orient == UP)
					plan.push_back(TURNRIGHT);
				else if (orient == DOWN)
					plan.push_back(TURNLEFT);
				else if (orient == LEFT)
				{
					plan.push_back(TURNLEFT);
					plan.push_back(TURNLEFT);
				}
			}
			else if (directionfound == D_UP || directionfound == D_LEFTUP || directionfound == D_RIGHTUP)
			{
				// orient up
				if (orient == LEFT)
					plan.push_back(TURNRIGHT);
				else if (orient == RIGHT)
					plan.push_back(TURNLEFT);
				else if (orient == DOWN)
				{
					plan.push_back(TURNLEFT);
					plan.push_back(TURNLEFT);
				}
			}
			else if (directionfound == D_DOWN || directionfound == D_RIGHTDOWN || directionfound == D_LEFTDOWN)
			{
				// orient down
				if (orient == RIGHT)
					plan.push_back(TURNRIGHT);
				else if (orient == LEFT)
					plan.push_back(TURNLEFT);
				else if (orient == UP)
				{
					plan.push_back(TURNLEFT);
					plan.push_back(TURNLEFT);
				}
			}

			// and then shoot!
			plan.push_back(SHOOT);
			worldState.agentHasArrow = false;
		}
	}
	if (plan.empty())
	{
		if (CanReachSafeUnvisitedLocation()) {
			// Agent should visit all safe locations reachable via safe locations
			// If CanReachSafeUnvisitedLocation returns true, then plan added as side effect
			// So, nothing else to do
		} else {
			// Give up (safe route to home should always exist)
			PlanSafeRoute(home);
			plan.push_back(CLIMB);
		}
	}
	action = plan.front();
	plan.pop_front();
	ExecuteAction (action);
	return action;
}

void Agent::UpdateState(Percept& percept)
{
	// Update wumpus health
	if (percept.Scream)
	{
		worldState.wumpusAlive = false;
	}
	// Update safe locations
	int x = worldState.agentLocation.X;
	int y = worldState.agentLocation.Y;
	safeLocations[x][y] = true; // current location must be safe
	if (((! percept.Stench) || (! worldState.wumpusAlive)) &&
		(! percept.Breeze))
	{
		// Adjacent locations are safe
		if (x > 1)
			safeLocations[x-1][y] = true;
		if (x < 4)
			safeLocations[x+1][y] = true;
		if (y > 1)
			safeLocations[x][y-1] = true;
		if (y < 4)
			safeLocations[x][y+1] = true;
	}
	if (percept.Stench)
		stenchLocations[x][y] = true;
}

void Agent::ExecuteAction(Action action)
{
	Orientation orient = worldState.agentOrientation;
	if (action == FORWARD)
	{
		// Assume not facing wall
		if (orient == UP)
			worldState.agentLocation.Y++;
		if (orient == DOWN)
			worldState.agentLocation.Y--;
		if (orient == LEFT)
			worldState.agentLocation.X--;
		if (orient == RIGHT)
			worldState.agentLocation.X++;
		visitedLocations[worldState.agentLocation.X][worldState.agentLocation.Y] = true;
	} else if (action == TURNLEFT) {
		if (orient == UP)
			worldState.agentOrientation = LEFT;
		if (orient == DOWN)
			worldState.agentOrientation = RIGHT;
		if (orient == LEFT)
			worldState.agentOrientation = DOWN;
		if (orient == RIGHT)
			worldState.agentOrientation = UP;
	} else if (action == TURNRIGHT) {
		if (orient == UP)
			worldState.agentOrientation = RIGHT;
		if (orient == DOWN)
			worldState.agentOrientation = LEFT;
		if (orient == LEFT)
			worldState.agentOrientation = UP;
		if (orient == RIGHT)
			worldState.agentOrientation = DOWN;
	} else if (action == GRAB) {
		// Assume gold is here
		worldState.agentHasGold = true;
	} else if (action == SHOOT) {
		worldState.agentHasArrow = false;
	} else if (action == CLIMB) {
		// Nothing to do here; either leave game if in [1,1] else no effect
	}
}

bool Agent::CanReachSafeUnvisitedLocation()
{
	int x, y;
	Location goalLocation;

	for (x = 1 ; x <= 4; x++)
	{
		for (y = 1; y <= 4; y++)
		{
			if ((safeLocations[x][y] && (! visitedLocations[x][y])))
			{
				goalLocation.X = x;
				goalLocation.Y = y;
				if (PlanSafeRoute(goalLocation))
					return true;
			}
		}
	}
	return false;
}

bool Agent::PlanSafeRoute(Location& goalLocation)
// Attempts to find route from agent's current location to the given goalLocation
// that passes through only known safe locations. If route found, then returns true
// and actions to get there are put on plan; otherwise, returns false and no change
// to plan. This is breadth-first search.
{
	list<SearchState*> frontier;
	list<SearchState*> explored;
	SearchState* initialState = new SearchState(worldState.agentLocation, worldState.agentOrientation, FORWARD, NULL);
	SearchState* currentState;
	SearchState* nextState;

	if (initialState->location == goalLocation) // goal test
		return true; // nothing added to plan
	frontier.push_back (initialState);
	while (! frontier.empty())
	{
		currentState = frontier.front();
		frontier.pop_front();
		explored.push_back (currentState);
		// Try actions: FORWARD=0, TURNLEFT=1, TURNRIGHT=2
		for (int action = 0; action < 3; action++)
		{
			if ((nextState = ChildState (currentState, (Action) action)) != NULL)
			{
				if (nextState->location == goalLocation)
				{
					AddRouteToPlan(nextState);
					ClearStates (frontier);
					ClearStates (explored);
					return true;
				} else if (nextState->location.X < 5 && nextState->location.Y < 5 &&
						   nextState->location.X > 0 && nextState->location.Y > 0) {
					if (!StateExistsIn(nextState, explored) && !StateExistsIn(nextState, frontier))
						frontier.push_back(nextState);
				}
			}
		}
	}
	ClearStates (explored);
	return false;
}

bool Agent::StateExistsIn(SearchState* state, list<SearchState*> stateList)
{
	list<SearchState*>::iterator it;
	for (it = stateList.begin(); it != stateList.end(); it++)
	{
		if (it._Ptr->_Myval->location == state->location &&
			it._Ptr->_Myval->orientation == state->orientation)
			return true;
	}
	return false;
}

void Agent::AddRouteToPlan(SearchState* state)
{
	list<Action> route;
	list<Action>::iterator itr;

	while (state->parent != NULL)
	{
		route.push_front(state->action);
		state = state->parent;
	}
	for (itr = route.begin(); itr != route.end(); itr++)
	{
		plan.push_back(*itr);
	}
}

SearchState* Agent::ChildState(SearchState* state, Action action)
{
	int x, y;
	SearchState* childState = NULL;
	Location newLocation;

	if (action == TURNLEFT)
	{
		childState = new SearchState (state->location, state->orientation, TURNLEFT, state);
		switch (state->orientation)
		{
			case UP: childState->orientation = LEFT; break;
			case DOWN: childState->orientation = RIGHT; break;
			case LEFT: childState->orientation = DOWN; break;
			case RIGHT: childState->orientation = UP; break;
		}
	}
	if (action == TURNRIGHT)
	{
		childState = new SearchState (state->location, state->orientation, TURNRIGHT, state);
		switch (state->orientation)
		{
			case UP: childState->orientation = RIGHT; break;
			case DOWN: childState->orientation = LEFT; break;
			case LEFT: childState->orientation = UP; break;
			case RIGHT: childState->orientation = DOWN; break;
		}
	}
	if (action == FORWARD)
	{
		x = state->location.X;
		y = state->location.Y;
		switch (state->orientation)
		{
			case UP: y++; break;
			case DOWN: y--; break;
			case LEFT: x--; break;
			case RIGHT: x++; break;
		}
		if (safeLocations[x][y])
		{
			newLocation.X = x;
			newLocation.Y = y;
			childState = new SearchState (newLocation, state->orientation, FORWARD, state);
		}
	}
	return childState;
}

void Agent::ClearStates(list<SearchState*> stateList)
{
	SearchState* tmpState;
	list<SearchState*>::iterator itr;
	for (itr = stateList.begin(); itr != stateList.end(); itr++)
	{
		tmpState = *itr;
		delete tmpState;
	}
}
