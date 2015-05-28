// Agent.h
// Christian Webber
// CptS 440
// September 10th, 2014
// HW 1

#ifndef AGENT_H
#define AGENT_H

#include "Action.h"
#include "Percept.h"

enum AgentPercepts {UNKNOWN, CLEAR, STENCH, WUMPUSMAYBE};
enum AgentFacing {FLEFT, FRIGHT, FUP, FDOWN};

class Agent
{
public:
	Agent ();
	~Agent ();
	void Initialize ();
	Action Process (Percept& percept);
	void GameOver (int score);
};

#endif // AGENT_H
