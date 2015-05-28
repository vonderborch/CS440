// Agent.h
// Christian Webber
// CptS 440
// HW 3

#ifndef AGENT2s_H
#define AGENT2s_H

#include "Action.h"
#include "Percept.h"
#include "WorldState.h"
#include <list>

enum Direction { D_NONE, D_UP, D_DOWN, D_LEFT, D_RIGHT, D_LEFTUP, D_LEFTDOWN, D_RIGHTUP, D_RIGHTDOWN };

class SearchState
{
public:
	SearchState (Location& loc, Orientation orient, Action act, SearchState* par);
	bool operator== (const SearchState& searchState);
	Location location;
	Orientation orientation;
	Action action; // action used to get from parent state to this state
	SearchState* parent;
};

class Agent
{
public:
	Agent();
	~Agent();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);

	// HW2 (assumes 4x4 world)
	void UpdateState (Percept& percept);
	void ExecuteAction (Action action);
	bool CanReachSafeUnvisitedLocation();
	bool PlanSafeRoute(Location& goalLocation);
	bool StateExistsIn(SearchState* state, list<SearchState*> stateList);
	void AddRouteToPlan (SearchState* state);
	SearchState* ChildState (SearchState* state, Action action);
	void ClearStates (list<SearchState*> stateList);

	list<Action> plan;
	WorldState worldState;
	bool safeLocations[5][5]; // only use [1][1] to [4][4]
	bool visitedLocations[5][5];
	bool stenchLocations[5][5];
};

#endif // AGENT_H
