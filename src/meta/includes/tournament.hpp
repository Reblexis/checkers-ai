//
// Created by cihalvi on 12/15/23.
//

#ifndef CHECKERS_TOURNAMENT_HPP
#define CHECKERS_TOURNAMENT_HPP

#include <vector>

#include "../../ai/includes/agent.hpp"
#include "../../app/includes/app.hpp"

const std::filesystem::path TOURNAMENT_LOGS_PATH = DATA_PATH / "tournament_logs/";

constexpr int MAX_MOVES = 150;

class Tournament{
private:
    std::string id;
    bool visualize;
    bool save{};
public:
    Tournament(std::string id, bool visualize = false);

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const;
    void simulateMatch(Agent *agent1, Agent *agent2, int randomMovesCount) const;
    void randomMatches(std::vector<Agent *> &agents, int matches, int randomMovesCount, std::optional<int> focusAgent = std::nullopt) const;
};

#endif //CHECKERS_TOURNAMENT_HPP
