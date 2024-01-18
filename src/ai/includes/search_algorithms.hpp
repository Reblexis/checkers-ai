#ifndef SEARCH_ALGORITHM_HPP
#define SEARCH_ALGORITHM_HPP

#include "../../communication/includes/debugging.hpp"
#include "../../communication/includes/game.hpp"
#include "../../meta/includes/timer.hpp"
#include "hyperparameters.hpp"
#include "cache.hpp"
#include "evaluation.hpp"
#include <utility>
#include <vector>

class SearchAlgorithm {
public:
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) = 0;
    virtual ~SearchAlgorithm() = default;
};

class Minimax : public SearchAlgorithm {
private:
    bool useAlphaBeta;
    bool useCache;
    bool reorderMoves;
    int maxDepth;
    long long moveTimeLimit; // Milliseconds
    Evaluation &evaluation;
    Cache<> cache; // Assuming template specialization if needed

public:
    Minimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> minimax(Game &game, const Timer& timer, int leftDepth, int alpha = INT32_MIN, int beta = INT32_MAX);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
    void setMaxDepth(int newDepth);
};

class IterativeMinimax : public SearchAlgorithm {
private:
    Minimax minimax;
    int maxDepth;
    long long moveTimeLimit;

public:
    IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
};

class RandomSearch : public SearchAlgorithm {
public:
    RandomSearch();
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
};

#endif // SEARCH_ALGORITHM_HPP
