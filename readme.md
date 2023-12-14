# Checkers AI
This repository contains a bot for the game called Checkers. It will also contain a graphical interface using which 
you can not only test out different agents but also play against them yourself.
We assume the following rules: https://www.officialgamerules.org/checkers . 

## External libraries
- SFML 2.5.1 (https://www.sfml-dev.org/download/sfml/2.5.1/)

## Current state
- [x] Implemented the rules of the game into move generation
- [x] Basic agent that uses minimax strategy with alpha-beta pruning and caching into RAM
- [x] Simple state evaluation algorithm
- [x] Visualization of moves in the console

## The plan
- [x] Improve the structure of the project
- [ ] Add documentation to the project
- [x] Implement a graphical interface for a possible visualization of the simulated games and for playing against an agent
- [x] Hyperparameters controlling different aspects of an agent (fx. depth of search, evaluation function, strategy, etc.)
- [ ] Agent saving (including an executable and a json file containing the hyperparameters)
- [ ] Caching into files (efficient storing of optimal moves for different game states)
- [ ] Implement basic agent versioning (saving the best agents, comparing their performance with different hyperparameters etc.)
