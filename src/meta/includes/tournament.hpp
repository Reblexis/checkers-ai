//
// Created by cihalvi on 12/15/23.
//

#ifndef CHECKERS_TOURNAMENT_HPP
#define CHECKERS_TOURNAMENT_HPP

#include <vector>

#include "statistics.hpp"
#include "../../ai/includes/agent.hpp"
#include "../../app/includes/app.hpp"

const std::filesystem::path TOURNAMENT_LOGS_PATH = DATA_PATH / "tournament_logs/";

constexpr int MAX_MOVES = 150;

class Tournament{
private:
    std::string id;
    bool visualize;
    int timeLimit;
    int randomStartMoves;
public:
    Tournament(std::string id, bool visualize = false, int timeLimit = 120, int randomStartMoves = 6);

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const;
    void simulateMatch(Agent *agent1, Agent *agent2) const;
    void randomMatches(std::vector<Agent *> &agents, int matches, std::optional<int> focusAgent = std::nullopt) const;
};

#endif //CHECKERS_TOURNAMENT_HPP
