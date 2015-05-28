// Agent.cc
// Christian Webber
// CptS 440
// September 10th, 2014
// HW 1

#include <iostream>
#include "Agent.h"

using namespace std;
int goalx = 0, goaly = 0,
	tgoalx = 0, tgoaly = 0,
	mex = 0, mey = 0;
AgentFacing directionfacing = FLEFT, directionin = FRIGHT;
bool haskilledwumpus = false, hasusedarrow = false, backagain = false;


Agent::Agent () { }

Agent::~Agent () { }

void Agent::Initialize ()
{
	goalx = goaly = 3;
	tgoalx = tgoaly = 0;
	mex = mey = 0;
	directionfacing = directionin = FRIGHT;
	haskilledwumpus = hasusedarrow = false;
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
	// setup the default action
	Action action = FORWARD;

	// have we reached our goal?
	if (goalx == mex && goaly == mey)
	{
		// are we going for the glitter or the escape?
		if (mex == 0 && mey == 0)
			action = CLIMB;
		else if (percept.Glitter)
		{
			action = GRAB;
			goalx = goaly = 0;
			directionin = directionfacing;
		}
	}

	// have we reached our temporary goal?
	if (tgoalx == mex && tgoaly == mey)
	{
		tgoalx = mex;
		tgoaly = mey;

		// if we have, we need a new x coord goal..
		if (mex < goalx)
			tgoalx++;
		else if (mex > goalx)
			tgoalx--;

		// and we need a new y coord goal...
		if (mey < goaly)
			tgoaly++;
		else if (mey > goaly)
			tgoaly--;

		// bounds checking
		if (tgoalx > 3)
			tgoalx = 3;
		else if (tgoalx < 0)
			tgoalx = 0;

		if (tgoaly > 3)
			tgoaly = 3;
		else if (tgoaly < 0)
			tgoaly = 0;
	}

	// is there a wumpus nearby (assuming we're not already climbing or grabbing)
	if (percept.Stench && action != CLIMB && action != GRAB && !haskilledwumpus)
	{
		// first we try to kill it
		if (!hasusedarrow)
		{
			action = SHOOT;
			hasusedarrow = true;
		}

		// detect a kill
		if (percept.Scream && hasusedarrow)
			haskilledwumpus = true;

		if (action != SHOOT)
		{
			// set temp goal to a diagonal
			if (mex < goalx)
				tgoalx--;
			else if (mex > goalx)
				tgoalx++;
			if (mey < goaly)
				tgoaly++;
			else if (mey > goaly)
				tgoaly--;

			if (!mey)
				tgoalx--;
			else if (mey == 3)
				tgoalx++;
			if (!mex)
				tgoaly--;
			else if (mex == 3)
				tgoaly++;

			// tgoal bounds checking
			if (tgoalx == -1)
				tgoalx = 0;
			if (tgoalx == 4)
				tgoalx = 3;
			if (tgoaly == 4)
				tgoaly = 3;
			if (tgoaly == -1)
				tgoaly = 0;

			// are we at 0,0?
			if (mex == 0 && mey == 0)
			{
				tgoalx = 1;
				tgoaly = 0;
			}
			// or at 3,3?
			else if (mex == 3 && mey == 3)
			{
				if (directionin == FUP)
				{
					tgoalx = 3;
					tgoaly = 2;
				}
				else if (directionin == FRIGHT)
				{
					tgoalx = 2;
					tgoaly = 3;
				}
			}
		}
	}

	// go to the temp goal
	if (action != SHOOT && action != GRAB)
	{
		// Y PART
		if (mey < tgoaly && directionfacing != FUP)
		{
			if (directionfacing == FLEFT)
				action = TURNRIGHT;
			else if (directionfacing == FRIGHT)
				action = TURNLEFT;
			else if (directionfacing == FDOWN)
				action = TURNLEFT;
		}
		else if (mey > tgoaly && directionfacing != FDOWN)
		{
			if (directionfacing == FLEFT)
				action = TURNLEFT;
			else if (directionfacing == FRIGHT)
				action = TURNRIGHT;
			else if (directionfacing == FUP)
				action = TURNRIGHT;
		}
		else if (mey < tgoaly && directionfacing == FUP)
			action = FORWARD;
		else if (mey > tgoaly && directionfacing == FDOWN)
			action = FORWARD;


		// X PART
		// turn to go towards the x goal
		if (mex < tgoalx && directionfacing != FRIGHT)
		{
			if (directionfacing == FLEFT)
				action = TURNRIGHT;
			else if (directionfacing == FUP)
				action = TURNRIGHT;
			else if (directionfacing == FDOWN)
				action = TURNLEFT;
		}
		else if (mex > tgoalx && directionfacing != FLEFT)
		{
			if (directionfacing == FRIGHT)
				action = TURNLEFT;
			else if (directionfacing == FUP)
				action = TURNLEFT;
			else if (directionfacing == FDOWN)
				action = TURNRIGHT;
		}

		// go farward
		else if (mex < tgoalx && directionfacing == FRIGHT)
			action = FORWARD;
		else if (mex > tgoalx && directionfacing == FLEFT)
			action = FORWARD;
	}

	// update mex and mey
	if (action == FORWARD)
	{
		if (directionfacing == FUP)
			mey++;
		if (directionfacing == FDOWN)
			mey--;
		if (directionfacing == FRIGHT)
			mex++;
		if (directionfacing == FLEFT)
			mex--;

		if (mex > 3)
			mex = 3;
		else if (mex < 0)
			mex = 0;
		if (mey > 3)
			mey = 3;
		else if (mey < 0)
			mey = 0;
	}
	// update direction facing
	if (action == TURNLEFT || action == TURNRIGHT)
	{
		if (directionfacing == FRIGHT && action == TURNLEFT)
			directionfacing = FUP;
		else if (directionfacing == FUP && action == TURNLEFT)
			directionfacing = FLEFT;
		else if (directionfacing == FLEFT && action == TURNLEFT)
			directionfacing = FDOWN;
		else if (directionfacing == FDOWN && action == TURNLEFT)
			directionfacing = FRIGHT;

		else if (directionfacing == FRIGHT && action == TURNRIGHT)
			directionfacing = FDOWN;
		else if (directionfacing == FDOWN && action == TURNRIGHT)
			directionfacing = FLEFT;
		else if (directionfacing == FLEFT && action == TURNRIGHT)
			directionfacing = FUP;
		else if (directionfacing == FUP && action == TURNRIGHT)
			directionfacing = FRIGHT;
	}

	// return the selected action
	return action;
}

void Agent::GameOver (int score)
{

}

