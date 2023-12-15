//
// Created by cihalvi on 12/15/23.
//

#ifndef CHECKERS_TOURNAMENT_HPP
#define CHECKERS_TOURNAMENT_HPP

#include <vector>

#include "../../ai/includes/agent.hpp"

constexpr int MAX_MOVES = 75;

class Tournament{
private:

public:
    Tournament();

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game);
    void simulateMatch(Agent *agent1, Agent *agent2, int randomMovesCount);
    void randomMatches(std::vector<Agent *> &agents, int matches, int randomMovesCount);
};

#endif //CHECKERS_TOURNAMENT_HPP
