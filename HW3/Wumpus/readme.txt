Wumpus Simulator v2.5 (released 9/13/2012)

Copyright (c) 2012. Washington State University.

Written by Larry Holder (holder@wsu.edu).

The Wumpus Simulator is a simple C++ framework for simulating the Wumpus World
describing in Russell and Norvig's "Artificial Intelligence: A Modern
Approach". The idea is for you to modify the Agent.h and Agent.cc files to
implement your super-smart agent. The agent provided simply accepts commands
from the keyboard and executes them in the simulator. The keyboard commands are
'f' for forward, 'l' for turnleft, 'r' for turnright, 'g' for grab, 's' for
shoot, and 'c' for climb.

Quick Start
-----------

To try out the simulator, install the code on a UNIX system (or a system that
has the 'make' program installed and a C++ compiler). Type 'make' to build the
'wumpsim' executable. Then, type './wumpsim'. You should see a randomly-generated 4x4 world, information about the game state, and a prompt for the next action.When the game is over, scoring information is provided.

Simulator Options
-----------------

The wumpus simulator takes a few options, as described below.

-size <N> lets you to set the size of the world to NxN (N>1). Default is 4.

-trials <N> runs the simulator for N trials, where each trial generates a new
wumpus world. Default is 1.

-tries <N> runs each trial of the simulator N times, giving your agent multiple
tries at the same world. Default is 1.

-seed <N> lets you set the random number seed to some positive integer so that
the simulator generates the same sequence of worlds each run. By default, the
random number seed is set semi-randomly based on the clock.

-world <file> lets you play a specific world as specified in the given file.
The -size option are ignored, and each try and trial uses the same world. The
format of the world file is as follows (all lowercase, must appear in this
order):

size N
wumpus N N
gold N N
pit N N
pit N N
...

where N is a positive integer. Some error checking is performed. A sample
world file is provided in testworld.txt.

Simulator Details
-----------------

The simulator works by generating a new world and a new agent for each trial.
Before each try on this world, the agent's Initialize() method is called, which
you can use to perform any pre-game preparation. Then, the game starts.  The
agent's Process() method is called with the current Percept, and the agent
should return an action, which is performed in the simulator. This continues
until the game is over (agent dies or leaves cave) or the maximum number of
moves (1000) is exceeded. When the game is over, the Agent's GameOver() method
is called. If additional tries are left for this world, then the world is
re-initialized, and the agent's Initialize() method is called again, and play
proceeds on another instance of the same game.

After the number of tries is completed, the agent is deleted. So, you may want
to store some information in the agent's destructor method to be reloaded
during the agent's constructor method when reborn for the next trial. If
additional trials have been requested, then a new wumpus world is generated,
and the process continues as described above.

Scoring information is output at the end of each try, each trial and at the end
of the simulation.

Agent Details
-------------

Your agent must include at least four methods: constructor, destructor,
Initialize, Process, and GameOver. You may change any or all of these methods
to implement your agent. And you may include additional methods as you see fit.
You may also take advantage of the other classes defined for the simulator
(e.g., Percept, Action, Orientation, Location, WorldState, WumpusWorld) by
including them directly, subclassing them, or borrowing code for your agent.
Once your agent is complete, simply type 'make' to remake the simulator with
your agent. Then run 'wumpsim' to try it out.

Happy hunting!

History
-------
Version 2.5 (released 9/13/2012)
- Change world generation and world input file error checking so that gold
  cannot be in location (1,1) to be consistent with Russell and Norvig text
- Prints 'x' in location of dead wumpus; stench perceived in in wumpus
  location and adjacent locations, whether dead or alive
Version 2.4 (released 11/2/2011)
- Added GameOver() method to Agent class, which is called after the end of
  each game with the game's final score
Version 2.3 (released 10/26/2011)
- Added Write method to WumpusWorld so you can write the current world to a
  file
- Removed leading underscore from WumpusWorld variables, since they are now
  public
Version 2.2 (released 10/4/2011)
- Reorganized header file inclusion to avoid errors using the WorldState
  class in the Agent.
Version 2.1 (released 9/20/2011)
- Fixed some #include issues to work with other platforms
- Changed random/srandom to rand/srand for Windows compatibility
- Added "-world <file>" option to allow playing a specific world
Version 2.0 (released 9/19/2011)
- Initial release
