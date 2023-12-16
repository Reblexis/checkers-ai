# User documentation
Welcome to the user documentation for the checkers project. This documentation will explain how to use the project without the need to write any code. Please read the [readme.md](../readme.md) file before this one
to get familiar with the project.

## App
As a user, you have limited options to interact with the system. By default, once you build the project, you will
be asked to provide paths for two agents (their folders), by default agent1 plays for the black color first. If you type: `human` instead of a path, you will be the agent. 
Then you will be asked how many games you want to be played, to that you answer with a number.

After-wards an app will open, where the games will be played. If you chose to play (by typing `human`), you will be
able to control the pieces once it is your turn by selecting them and the target squares.

After all games are finished, you will get the final statistics in the console (agent 1 wins, agent 2 wins, draws).

## Agents
Each agent has its own folder. In the folder, there should always be a file called `hyperparameters.json`, which controls 
the agent's behavior (you can find an example of such file in the [data/default_hyperparameters.json](../data/default_hyperparameters.json) file) or an executable (called `agent`)
adhering to the desired protocol.

### Protocol
The agent executable should be able to receive the current game state and respond with the desired move.
The output contains the move and the board state as well (board serves for simple reading of the new state and the move serves as a proof of the validity of the move).

#### Input
- **Line 1: color**. The first line will either be `black` or `white` representing for which color should the agent play.
- **Lines 2-9: board**. The next 8 lines will contain the board state. Each line will contain 8 characters separated by space, each representing
a square on the board. The characters will be either `.` (empty square), `b` (black piece), `w` (white piece), `B` (black king), `W` (white king).
The board is from white's perspective (white pawns can move up, black pawns can move down).

#### Output
- **Line 1: move**. The first line will contain the move the agent wants to make.
First number `n` will represent the amount of different positions the agent visits during the move (including the starting and ending one).
The next `n` pairs of numbers will represent the visited positions. The first number in the pair will represent the column (from 0 to 7 left to right), 
the second number will represent the row (from 0 to 7 top to bottom).
- **Line 2: resulting board**. The second line will contain the resulting board state after the move. The format is the same as the input format.