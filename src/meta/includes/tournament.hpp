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

enum class TournamentType{
    roundRobin,
    randomMatches
};

const std::map<std::string, TournamentType> TOURNAMENT_TYPE_MAP = {
        {"roundRobin", TournamentType::roundRobin},
        {"randomMatches", TournamentType::randomMatches}
};


/**
 * @class Tournament
 * @brief Manages tournament between all kinds of agents.
 *
 * The tournament class is used to manage a tournament between agents.
 * The tournament can be run in two different ways:
 * - roundRobin: Agents play each other in a round robin format.
 * - randomMatches: Agents play a set number of random matches.
 *
 * The tournament can be visualized and has a time limit and maximum number of moves per game.
 *
 * @var id The id of the tournament (used for logging)
 * @var agents The agents participating in the tournament
 * @var tournamentType The type of the tournament
 * @var visualize Whether the tournament should be visualized (using GUI)
 * @var timeLimit The time limit for each game
 * @var maxMoves The maximum number of moves per game
 */
class Tournament{
private:
    std::string id;
    std::vector<std::unique_ptr<Agent>> agents;

    TournamentType tournamentType;
    bool visualize;
    int timeLimit;
    int maxMoves;

    void simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const; ///< Simulates a game between two agents
    void simulateMatch(Agent *agent1, Agent *agent2) const; ///< Simulates a match between two agents (two games with reversed colors)
    void randomMatches(int matches=-1, std::optional<int> focusAgent = std::nullopt) const; ///< Simulates a set number of random matches between agents
    void roundRobin() const; ///< Simulates a round robin tournament between agents
    void launch(); ///< Launches the tournament

public:
    explicit Tournament(std::string id, std::vector<std::unique_ptr<Agent>> &&agents, TournamentType tournamentType, bool visualize = false, int timeLimit = 60, int maxMoves = 100);
    static Tournament createFromFile(const std::filesystem::path &path); ///< Creates a tournament from a JSON file
};

#endif //CHECKERS_TOURNAMENT_HPP
