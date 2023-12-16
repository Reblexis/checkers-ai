# Checkers AI
This repository contains an implementation of the checkers game and agents that can play it (with many different strategies).
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
### Windows
```
cmake CMakeLists.txt
cmake --build . --config Release
```

## Current state
- [x] Implemented the rules of the game into move generation
- [x] Agent using iterative-deepening, alpha-beta pruning, cache and move reordering
- [x] Simple state evaluation algorithm
- [x] Visualization of moves in the console
- [x] Graphical interface for playing against an agent (or another player) or watching agents fight each other
- [x] Hyperparameters controlling different aspects of an agent (fx. depth of search, evaluation function, strategy, etc.)
- [x] Agent saving
- [x] Tournament system for agents comparison

## Documentation
If you want to learn more about the project (and how to use it), you can find the documentation in the [documentation](documentation) folder. 

## Contact
If you have any questions, please contact me at: `viktor.cihal@gmail.com` or write it in the issues section of this repository.
