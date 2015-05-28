// Agent.h
// Christian Webber
// CptS 440
// September 10th, 2014
// HW 2

#ifndef AGENT_H
#define AGENT_H

#include <fstream>
#include "Action.h"
#include "Percept.h"

enum AgentKnowledge { AK_UNKNOWN, AK_EDGE, AK_SAFE, AK_STENCH, AK_WUMPUS, AK_WUMPUSMAYBE, AK_BREEZE, AK_PIT, AK_PITMAYBE};
enum AgentFacing { AF_LEFT, AF_RIGHT, AF_UP, AF_DOWN, AF_FAKE };
struct AgentCoords
{
	int x;
	int y;
};
struct AgentTile
{
	vector<AgentKnowledge> knowledge;

	bool HasKnowledge(AgentKnowledge knowtodetermine)
	{
		for each (AgentKnowledge know in knowledge)
		{
			if (knowtodetermine == know)
				return true;
		}

		return false;
	}
};

class AgentSearchNode
{
protected:
	AgentCoords coords, goal, parentcoords;
	AgentFacing direction, parentdirection;
	double cost;
	vector<vector<AgentTile>> world;
	bool wumpusdead = false;
	// Weights for different things
	int av_w_stench = 10, av_w_breeze = 10, av_w_unknown = 1, av_w_safe = 10, av_w_staying = 1;

public:
	AgentSearchNode()
	{
		coords.x = coords.y = -1;
		goal.x = goal.y = -1;
		cost = 100000000;
		parentcoords.x = parentcoords.y = -1;
		parentdirection = AF_LEFT;
	}
	AgentSearchNode(int x, int y, AgentCoords goals)
	{
		coords.x = x;
		coords.y = y;
		cost = 100000000;
		goal = goals;
		parentcoords.x = parentcoords.y = -1;
		parentdirection = AF_LEFT;
	}
	~AgentSearchNode() { };

	bool IsGoal()
	{
		if (coords.x == goal.x && coords.y == goal.y)
			return true;

		return false;
	}

	vector<AgentSearchNode> GetSuccessors()
	{
		//////////// setup variables ////////////
		vector<AgentSearchNode> successors;
		AgentSearchNode forward, left, right;
		AgentCoords tempcoords, parentc = coords;
		AgentFacing tempfacing = GetDirection(), parentf = GetDirection();
		double tempcost = 0;

		//////////// calc forward ////////////
		// determine successor's coords
		tempcoords = GetCoords();
		if (tempfacing == AF_LEFT)
			tempcoords.x--;
		else if (tempfacing == AF_RIGHT)
			tempcoords.x++;
		else if (tempfacing == AF_DOWN)
			tempcoords.y--;
		else if (tempfacing == AF_UP)
			tempcoords.y++;
		if (tempcoords.x < 0)
			tempcoords.x = 0;
		if (tempcoords.y < 0)
			tempcoords.y = 0;
		// setup successor
		forward.SetCoords(tempcoords);
		forward.SetCost(CalculateCost(tempcoords, coords, tempfacing));
		forward.SetDirection(tempfacing);
		forward.SetGoal(goal);
		forward.SetWorld(world);
		forward.SetWumpusDead(wumpusdead);
		forward.SetParentCoords(parentc);
		forward.SetParentDirection(parentf);

		tempcoords = GetCoords();
		//////////// calc left ////////////
		// determine successor's coords
		if (tempfacing == AF_LEFT)
			tempfacing = AF_DOWN;
		else if (tempfacing == AF_RIGHT)
			tempfacing = AF_UP;
		else if (tempfacing == AF_DOWN)
			tempfacing = AF_RIGHT;
		else if (tempfacing == AF_UP)
			tempfacing = AF_LEFT;
		// setup successor
		left.SetCoords(tempcoords);
		left.SetCost(CalculateCost(tempcoords, coords, tempfacing));
		left.SetDirection(tempfacing);
		left.SetGoal(goal);
		left.SetWorld(world);
		left.SetWumpusDead(wumpusdead);
		left.SetParentCoords(parentc);
		left.SetParentDirection(parentf);

		tempfacing = GetDirection();
		//////////// calc right ////////////
		// determine successor's coords
		if (tempfacing == AF_LEFT)
			tempfacing = AF_UP;
		else if (tempfacing == AF_RIGHT)
			tempfacing = AF_DOWN;
		else if (tempfacing == AF_DOWN)
			tempfacing = AF_LEFT;
		else if (tempfacing == AF_UP)
			tempfacing = AF_RIGHT;
		// setup successor
		right.SetCoords(tempcoords);
		right.SetCost(CalculateCost(tempcoords, coords, tempfacing));
		right.SetDirection(tempfacing);
		right.SetGoal(goal);
		right.SetWorld(world);
		right.SetWumpusDead(wumpusdead);
		right.SetParentCoords(parentc);
		right.SetParentDirection(parentf);

		//////////// push the successors to the vector and return it ////////////
		successors.push_back(forward);
		successors.push_back(left);
		successors.push_back(right);
		return successors;
	}

	double GetCost() { return cost; }
	AgentCoords GetCoords() { return coords; }
	AgentFacing GetDirection() { return direction; }
	AgentCoords GetParentCoords() { return parentcoords; }
	AgentFacing GetParentDirection() { return parentdirection; }

	void SetCost(double ncost) { cost = ncost; }
	void SetCoords(AgentCoords ncoords) { coords = ncoords; }
	void SetDirection(AgentFacing nfacing) { direction = nfacing; }
	void SetGoal(AgentCoords ngoal) { goal = ngoal; }
	void SetWorld(vector<vector<AgentTile>> nworld) { world = nworld; }
	void SetWumpusDead(bool nwumpusdead) { wumpusdead = nwumpusdead; }
	void SetParentCoords(AgentCoords ncoords) { parentcoords = ncoords; }
	void SetParentDirection(AgentFacing nfacing) { parentdirection = nfacing; }

	static int CalculateDistance(AgentCoords ncoords, AgentCoords ngoal)
	{
		int xdif = abs(ncoords.x - ngoal.x),
			ydif = abs(ncoords.y - ngoal.y);
		return xdif + ydif;
	}

	int CalculateCost(AgentCoords tempcoords, AgentCoords curcoords, AgentFacing facing = AF_FAKE)
	{
		// base cost of any action
		int tempcost = 1;
		// distance from goal^2
		tempcost += CalculateDistance(tempcoords, goal);

		// angular distance
		if (facing != AF_FAKE)
		{
			if (facing == AF_LEFT && goal.y == tempcoords.y)
				tempcost += 1;
			else if (facing == AF_LEFT)
				tempcost += 2;

			if (facing == AF_DOWN && goal.x == tempcoords.x)
				tempcost += 1;
			else if (facing == AF_DOWN)
				tempcost += 2;

			if (facing == AF_RIGHT && goal.y == tempcoords.y)
				tempcost += 1;
			else if (facing == AF_RIGHT)
				tempcost += 2;

			if (facing == AF_UP && goal.x == tempcoords.x)
				tempcost += 1;
			else if (facing == AF_UP)
				tempcost += 2;
		}

		// stench, breeze, unknown, etc. of new position if we're moving forward
		if (tempcoords.x != curcoords.x || tempcoords.y != curcoords.y)
		{
			// what percepts does the agent have about its current position?
			if (world.size() > curcoords.x)
			{
				if (world[curcoords.x].size() > curcoords.y)
				{
					if (world[curcoords.x][curcoords.y].HasKnowledge(AK_STENCH) && !wumpusdead)
						tempcost += av_w_stench;
					if (world[curcoords.x][curcoords.y].HasKnowledge(AK_BREEZE))
						tempcost += av_w_breeze;
					if (world[curcoords.x][curcoords.y].HasKnowledge(AK_UNKNOWN))
						tempcost += av_w_unknown;
				}
				else
					tempcost += av_w_unknown;
			}
			// does it know anything about the new location?
			if (world.size() > tempcoords.x)
			{
				if (world[tempcoords.x].size() > tempcoords.y)
				{
					if (world[tempcoords.x][tempcoords.y].HasKnowledge(AK_STENCH) && !wumpusdead)
						tempcost += av_w_stench;
					if (world[tempcoords.x][tempcoords.y].HasKnowledge(AK_BREEZE))
						tempcost += av_w_breeze;
					if (world[tempcoords.x][tempcoords.y].HasKnowledge(AK_UNKNOWN))
						tempcost += av_w_unknown;
					if (world[tempcoords.x][tempcoords.y].HasKnowledge(AK_SAFE))
						tempcost -= av_w_safe;
				}
				else
					tempcost += av_w_unknown;
			}
			else
				tempcost += av_w_unknown;
		}
		// if we're not moving, add a cost for staying put
		else
			tempcost += av_w_staying;

		// check to make sure cost >= 0
		if (tempcost < 0)
			tempcost = 0;

		// return the cost
		return tempcost;
	}
};

class Agent
{
	protected:
		bool av_b_wumpusdead = false, // is the wumpus dead?
			 av_b_arrowused = false, // has the agent used its arrow?
			 av_b_hasgold = false, // does the agent have the gold?
			 av_b_justshotarrow = false, // did the agent shoot the arrow last tick?
			 av_b_debugmode = true; // should the agent print out what it's doing?

		char* av_s_debugfile = "debug.txt"; // where the agent should print out what it's doing
		std::ofstream av_ofs_stream;

		// what direction is the agent currently facing?
		AgentFacing av_af_directionfacing = AF_RIGHT;

		// what are the agent's current goal coords and position?
		AgentCoords av_xy_goal, av_xy_position;

		// the list of actions the agent currently plans on doing to reach the goal.
		vector<Action> av_va_plan;

		// the agent's knowledge about the world it's living in
		vector<vector<AgentTile>> av_vat_world;

		// the last action that the agent took
		Action av_a_lastaction;

		// the current tick for the Agent
		int av_i_tick = 0;

	private:
		Action AgentTick(Percept& percept);
		void UpdatePosition(Action action);
		bool CreateKnowledge(Percept& percept);
		vector<Action> CreatePlan();
		vector<AgentCoords> CreatePlanSearch();
		vector<AgentCoords> CreatePlanSearchReconstructPath(vector<AgentSearchNode> camefrom, AgentSearchNode current);
		AgentSearchNode CreatePlanSearchGetCurrentParentInfo(AgentSearchNode node, vector<AgentSearchNode> set);
		bool CreatePlanSearchInSet(AgentSearchNode node, vector<AgentSearchNode> set);
		vector<Action> CreatePlanActions(vector<AgentCoords> coordplan);

		void DebugPrintTick();
		void DebugPrintWorld();
		void DebugPrintCoordPlan(vector<AgentCoords> coordslist);
		void DebugPrintActionsPlan();

	public:
		Agent ();
		~Agent ();
		void Initialize ();
		Action Process (Percept& percept);
		void GameOver (int score);
};

#endif // AGENT_H
