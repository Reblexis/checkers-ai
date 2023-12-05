#ifndef SEARCH_ALGORITHM_HPP
#define SEARCH_ALGORITHM_HPP

#include "../../communication/includes/debugging.hpp"
#include "../../communication/includes/game.hpp"
#include "hyperparameters.hpp"
#include "cache.hpp"
#include "evaluation.hpp"
#include <utility>
#include <vector>

class SearchAlgorithm {
public:
    virtual std::pair<int, piece_move> findBestMove(Game &game) = 0;
    virtual ~SearchAlgorithm() = default;
};

class Minimax : public SearchAlgorithm {
private:
    bool useAlphaBeta;
    bool useCache;
    int maxDepth;
    int operationLimit;
    int curOperations;
    Evaluation &evaluation;
    Cache<> cache; // Assuming template specialization if needed

public:
    Minimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> minimax(Game &game, int leftDepth, int alpha = INT32_MIN, int beta = INT32_MAX);
    std::pair<int, piece_move> findBestMove(Game &game) override;
    void resetOperations();
    void setMaxDepth(int newDepth);
    void setOperationLimit(int newLimit);
};

class IterativeMinimax : public SearchAlgorithm {
private:
    Minimax minimax;
    int maxDepth;
    int operationLimit;

public:
    IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> findBestMove(Game &game) override;
};

class RandomSearch : public SearchAlgorithm {
public:
    RandomSearch();
    std::pair<int, piece_move> findBestMove(Game &game) override;
};

#endif // SEARCH_ALGORITHM_HPP
