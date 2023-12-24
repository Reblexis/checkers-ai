//
// Created by cihalvi on 12/15/23.
//

#ifndef CHECKERS_TOURNAMENT_HPP
#define CHECKERS_TOURNAMENT_HPP

#include <vector>

#include "../../ai/includes/agent.hpp"
#include "../../app/includes/app.hpp"

constexpr int MAX_MOVES = 150;

class Tournament{
private:
    bool visualize;
public:
    Tournament(bool visualize = false);

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game);
    void simulateMatch(Agent *agent1, Agent *agent2, int randomMovesCount);
    void randomMatches(std::vector<Agent *> &agents, int matches, int randomMovesCount, std::optional<int> focusAgent = std::nullopt);
};

#endif //CHECKERS_TOURNAMENT_HPP
