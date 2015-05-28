// Agent.cc
// Christian Webber (11119437)
// CptS 440
// September 10th, 2014
// HW 1

#include <iostream>
#include "Agent.h"

using namespace std;

Agent::Agent ()
{

}

Agent::~Agent ()
{

}

void Agent::Initialize ()
{

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
Action Agent::Process (Percept& percept)
{
	char c;
	Action action;
	bool validAction = false;

	while (! validAction)
	{
		validAction = true;
		cout << "Action? ";
		cin >> c;
		if (c == 'f') {
			action = FORWARD;
		} else if (c == 'l') {
			action = TURNLEFT;
		} else if (c == 'r') {
			action = TURNRIGHT;
		} else if (c == 'g') {
			action = GRAB;
		} else if (c == 's') {
			action = SHOOT;
		} else if (c == 'c') {
			action = CLIMB;
		} else {
			cout << "Huh?" << endl;
			validAction = false;
		}
	}
	return action;
}

void Agent::GameOver (int score)
{

}

