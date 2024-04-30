# Checkers AI
This repository contains an implementation of the checkers game and agents that can play it (with many different strategies).
It also contains a tournament system using which you can test your checkers bots or play against them.
The project uses the following rules: https://www.officialgamerules.org/checkers .

## External libraries
- SFML 2.5.1 (https://www.sfml-dev.org/download/sfml/2.5.1/)

## How to build
Make sure that you have the SFML library installed on your system.
### Linux
```
cmake CMakeLists.txt
make
```
Then launch the executable.
```
cd out
./checkers
```
### There are no other platforms supported at the moment

## Current state
- [x] Implemented the rules of the game into move generation
- [x] Agent using iterative-deepening, alpha-beta pruning, cache and basic state evaluation
- [x] Evaluation and search algorithm abstractions
- [x] Visualization of moves in the console
- [x] Hyperparameters controlling different aspects of an agent (fx. depth of search, evaluation function, strategy, etc.)
- [x] Agent saving
- [x] Tournament system for agents and players comparison
- [x] Support for background search (while the other agent is thinking) 

## App
As a user, you have limited options to interact with the system. By default, you are
able to customize and create a tournament via the tournament config file (more on that in the tournament section below). 
You will provide basic tournament settings and also the competitors' paths and ids.
You have an option to visualize the matches via a graphical interface. 

![checkers_screenshot](https://github.com/Reblexis/checkers/assets/80317928/e26bcf98-468c-4bcc-bea5-31ff1b0795f6)


## Agents
Currently, there is an abstraction in-place that allows for 3 different types of agents.
- **Human**. This agent is controlled by a human player via the graphical interface. In order to specify this agent (when creating a tournament), you just need to give him a unique id.
- **Hyperparameters agent**. This agent uses local scripts defined in this repository. The strategy that it will use is defined by the hyperparameters file that you will provide (you can find an example of such file here [default_hyperparameters.json](../data/default_hyperparameters.json)).
- **Executable agent**. This type of agent allows for support of external agents, which may be provided in form of an executable that adheres to the specified protocol (see [protocol](#protocol)).


## Tournament
The tournament system allows for the comparison of different agents (including players and other executables implementing the protocol).
It is controlled by a config file (see example [default_tournament.json](data/default_tournament.json)) that specifies the tournament settings and the agents that will participate.
There are the following parameters that you have to specify:
- **id** - A unique identifier of the tournament. This will be then used to save the results of the tournament.
- **agents** - A list of agents that will participate in the tournament. Each agent should have a unique id and a path to the executable or a hyperparameters file (unless it is a player - then the path is not needed). There are the following types of agents you can specify: `player`, `hyperparameters`, `executable`.
- **tournamentType** - The type of the tournament. Currently, there are two types of tournaments supported: `roundRobin` (https://en.wikipedia.org/wiki/Round-robin_tournament) and `randomMatches` (matches are played between random agents until user termination).
- **timeLimit** - The time limit for each agent in the tournament (in milliseconds).
- **maxMoves** - The maximum amount of moves that can be played in a single match (if the game is not finished by then, the match is considered a draw).

The default setup in the [default_tournament.json](data/default_tournament.json) file allows for a simple match between an agent and a player.

## Statistics
Once you launch the tournament, you can view the tournament statistics under [data/tournament_logs/](data/tournament_logs/).
There you can find a folder by the tournament id, that you specified in the tournament config file. For each agent that participated in the tournament, you can find a file with the statistics of the agent's matches and rating.

### Protocol
The agent executable should be able to repeatedly receive the current game state information and respond with the desired move.
The output should contain the move that the agent wants to take.
By default, the board orientation is set in such a way, that the white pieces are at the bottom and the black pieces are at the top of the board.

#### Initial input
First the agent should accept the following lines:
- **Line 1: time left** - The total amount of time the agent has from the start (in milliseconds, as a whole number).
- **Line 2: color** - Either `black` or `white` representing for which color should the agent play.

#### Initial output
The agent outputs nothing as of this moment.

After-wards, the game starts and the game states are represented by:
#### Game loop input
- **Line 1: time left** - The amount of time the agent has left to make a move (in milliseconds as a whole number).
- **Lines 2-9: board**. The next 8 lines will contain the board state. Each line will contain 8 characters separated by space, each representing
  a square on the board. The characters will be either `.` (empty square), `b` (black piece), `w` (white piece), `B` (black king), `W` (white king).
  The board is from white's perspective (white pawns can move up, black pawns can move down).

For each game loop input, the agent should respond with:
#### Game loop output
- **Line 1: move**. The first line will contain the move the agent wants to make.
  First number `n` will represent the amount of different positions the agent visits during the move (including the starting and ending one).
  The next `n` pairs of numbers will represent the visited positions. The first number in the pair will represent the column (from 0 to 7 left to right),
  the second number will represent the row (from 0 to 7 top to bottom).

#### Example interaction
This is an example of a game where the agent plays as black and has 60 seconds in total for all moves. The initial input and two iterations of the game loop are shown.
```
// Initial input
60000
white
// Game loop input (1)
60000
. b . b . b . b 
b . b . b . b . 
. b . b . b . b 
. . . . . . . . 
. . . . . . . . 
w . w . w . w . 
. w . w . w . w 
w . w . w . w . 
// Game loop output (1)
2 3 2 2 3
// Game loop input (2)
59458
. b . b . b . b
b . b . b . b .
. b . . . b . b
. . b . . . . .
. . . . . . . w
w . w . w . . .
. w . w . w . w
w . w . w . w .
// Game loop output (2)
2 1 2 0 3
```
## Developer Documentation
If you want to learn more specific code details about the project you can generate the developer documentation using Doxygen once you clone the project.
```
doxygen Doxyfile
```
Then you can open the [documentation/html/index.html](documentation/html/index.html) file in your browser.

## Contact
If you have any questions or if you find any suspicious behaviour of the software, please contact me at: `viktor.cihal@gmail.com` or write it in the issues section of this repository.
