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

constexpr int MAX_MOVES = 200;

enum class TournamentType{
    ROUND_ROBIN,
    RANDOM_MATCHES
};

const std::map<std::string, TournamentType> TOURNAMENT_TYPE_MAP = {
        {"roundRobin", TournamentType::ROUND_ROBIN},
        {"randomMatches", TournamentType::RANDOM_MATCHES}
};


class Tournament{
private:
    std::string id;
    std::vector<std::unique_ptr<Agent>> agents;

    TournamentType tournamentType;
    bool visualize;
    int timeLimit;
    int randomStartMoves;

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const;
    void simulateMatch(Agent *agent1, Agent *agent2) const;
    void randomMatches(std::vector<std::unique_ptr<Agent>> &agents, int matches, std::optional<int> focusAgent = std::nullopt) const;
    void roundRobin(std::vector<std::unique_ptr<Agent>> &agents) const;
    void launch();

public:
    explicit Tournament(std::string id, std::vector<std::unique_ptr<Agent>> &&agents, TournamentType tournamentType, bool visualize = false, int timeLimit = 60, int randomStartMoves = 6);
    static Tournament createFromFile(const std::filesystem::path &path);
};

#endif //CHECKERS_TOURNAMENT_HPP
