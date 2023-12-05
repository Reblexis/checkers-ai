#ifndef AGENT_HPP
#define AGENT_HPP

#include <string>

#include "hyperparameters.hpp"
#include "search_algorithms.hpp"
#include "evaluation.hpp"

class Board; // Forward declaration (assuming Board is defined elsewhere)

class Agent {
private:
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    Hyperparameters hyperparameters;

public:
    ~Agent();
    explicit Agent(const std::filesystem::path &dataPath);
    std::pair<int, piece_move> findBestMove(Board &board);
};

#endif // AGENT_HPP
