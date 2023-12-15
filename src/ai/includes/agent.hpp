#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"
#include "statistics.hpp"

const std::filesystem::path AGENTS_PATH = DATA_PATH / "agents/";
const std::filesystem::path HYPERPARAMETERS_PATH = "hyperparameters.json";
const std::filesystem::path STATISTICS_PATH = "statistics.json";

class Agent {
private:
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    Hyperparameters hyperparameters;
    std::optional<Statistics> statistics;

public:
    void initialize();
    explicit Agent(const std::filesystem::path &dataPath);
    explicit Agent(const std::string &hyperparameters);
    std::pair<int, piece_move> findBestMove(Game &game);
    void addGame(const int enemyRating, double result);
    int getRating() const;
};

#endif // AGENT_HPP
