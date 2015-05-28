// Agent.h
// Christian Webber (11119437)
// CptS 440
// September 10th, 2014
// HW 1

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"
#include "WorldState.h"
#include <algorithm>
#include <list>
#include <vector>

// orientation with a none orientation
enum EnhancedOrientation { EO_NONE, EO_LEFT, EO_RIGHT, EO_UP, EO_DOWN };

// word info struct
struct WorldInfo
{
	bool _unknown,
		 _safe,
		 _stench,
		 _breeze,
		 _pit,
		 _wumpus;

	WorldInfo()
	{
		_unknown = true;
		_safe = _stench = _breeze = _wumpus = _pit = false;
	}

	void Visit(Percept& percept)
	{
		if (_unknown)
		{
			_unknown = false;
			_safe = true;

			if (percept.Breeze)
				_breeze = true;
			if (percept.Stench)
				_stench = true;
		}
	}
};

// node class used for searching
class SearchNode
{
public:
	// variables
	Location location, goal;
	Orientation orientation;
	Action action;
	SearchNode* parent;
	double cost, t_cost;
	vector<vector<WorldInfo>> agent_world;
	// functions
	SearchNode() { }
	~SearchNode()  { }
	SearchNode(Location& loc, Location& gl, Orientation orient, Action act, vector<vector<WorldInfo>> world, SearchNode* par)
	{
		location = loc;
		goal = gl;
		orientation = orient;
		action = act;
		parent = par;
		agent_world = world;
		cost = CalculateCost();
		// add cost of the parent
		if (parent != NULL)
			t_cost += parent->cost + cost;
	}
	bool operator== (const SearchNode& searchnode)
	{
		if (location == searchnode.location && orientation == searchnode.orientation)
			return true;
		return false;
	}
	bool operator!= (const SearchNode& searchnode)
	{
		if (location == searchnode.location && orientation == searchnode.orientation)
			return false;
		return true;
	}
	vector<SearchNode> GetSuccessors()
	{
		vector<SearchNode> output;
		for (int i = 0; i < 3; i++)
		{
			Location loc = location;
			Orientation orient = orientation;
			SearchNode* par = new SearchNode(*this);
			Action act;

			if (i == 0)
			{
				act = FORWARD;
				if (orient == LEFT)
					loc.X--;
				else if (orient == RIGHT)
					loc.X++;
				else if (orient == UP)
					loc.Y++;
				else
					loc.Y--;
			}
			else if (i == 1)
			{
				act = TURNLEFT;
				if (orient == LEFT)
					orient = DOWN;
				else if (orient == RIGHT)
					orient = UP;
				else if (orient == UP)
					orient = LEFT;
				else
					orient = RIGHT;
			}
			else if (i == 2)
			{
				act = TURNRIGHT;
				if (orient == LEFT)
					orient = UP;
				else if (orient == RIGHT)
					orient = DOWN;
				else if (orient == UP)
					orient = RIGHT;
				else
					orient = LEFT;
			}

			output.push_back(SearchNode(loc, goal, orient, act, agent_world, par));
		}
		return output;
	}

private:
	double CalculateCost()
	{
		int weight_unknown = 1, weight_breeze = 10, weight_stench = 10,
			weight_staying = 2, weight_impossible = 100;
		double output = 0;

		// add cost of the node
		int x = location.X - 1, y = location.Y - 1;

		if (x >= agent_world.size() || y >= agent_world[0].size() || x < 0 || y < 0)
			output += weight_impossible;
		else
		{
			if (agent_world[x][y]._unknown)
				output += weight_unknown;
			if (agent_world[x][y]._breeze)
				output += weight_breeze;
			if (agent_world[x][y]._stench)
				output += weight_stench;
		}
		if (parent != NULL)
		{
			if (location == parent->location)
				output += weight_staying;
		}

		int xdif = abs(location.X - goal.X),
			ydif = abs(location.Y - goal.Y);

		output += xdif + ydif;

		if (output < 0)
			output = 0;

		return output;
	}
};

// agent class!
class Agent3
{
	private:
		// variables
		WorldState agent_knowledge,
				   agent_knowledge_last;
		list<Action> agent_plan;
		vector<vector<WorldInfo>> agent_world;
		Location home, gold;

		// functions
		void ResizeWorld(int columns, int rows);
		Action Update(Percept& percept);
		bool UpdateWorld(Percept& percept);
		void UpdatePlan(Location destination, Percept& percept);
		Location GetNearestHorizontalVerticalLocation(Location destination);
		bool PlanRoute(Location destination, EnhancedOrientation orient);
		bool NodeExistsIn(SearchNode node, vector<SearchNode*> list);
		vector<SearchNode> ReconstructPath(SearchNode current, vector<SearchNode> came_from);

	public:
		// functions
		Agent3();
		~Agent3();
		void Initialize ();
		Action Process (Percept& percept);
		void GameOver (int score);
		bool LocationsNotEqual(Location one, Location two)
		{
			if (one == two)
				return false;
			return true;
		}
};

#endif // AGENT_H
