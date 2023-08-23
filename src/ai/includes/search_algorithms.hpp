#ifndef SEARCH_ALGORITHM_HPP
#define SEARCH_ALGORITHM_HPP

#include "../../communication/includes/debugging.hpp"
#include "../../communication/includes/interface.hpp"
#include "hyperparameters.hpp"
#include "cache.hpp"
#include "evaluation.hpp"

class SearchAlgorithm{
public:
    virtual std::pair<int, move> findBestMove(Board &board) = 0;
    virtual ~SearchAlgorithm() = default;
};

class Minimax: public SearchAlgorithm{
private:
    bool useAlphaBeta;
    bool useCache;
    int maxDepth;
    int operationLimit;

    int curOperations;
    Evaluation &evaluation;
    Cache<> cache;

public:
    Minimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, move> minimax(Board &board, int leftDepth, int alpha = INT32_MIN, int beta = INT32_MAX);
    std::pair<int, move> findBestMove(Board &b) override;
    void resetOperations();
    void setMaxDepth(int newDepth);
    void setOperationLimit(int newLimit);
};

class IterativeMinimax: public SearchAlgorithm{
private:
    Minimax minimax;
    int maxDepth;
    int operationLimit;

public:
    IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, move> findBestMove(Board &board) override;
};

class RandomSearch: public SearchAlgorithm{
public:
    RandomSearch();
    std::pair<int, move> findBestMove(Board &board) override;
};

#endif // SEARCH_ALGORITHM_HPP
