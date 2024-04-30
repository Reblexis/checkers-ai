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
#include <random>

/**
 * @class SearchAlgorithm
 * @brief Abstract class for search algorithms.
 *
 * Serves for finding the best move in a given game state.
 */
class SearchAlgorithm {
public:
    virtual std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) = 0;
    virtual ~SearchAlgorithm() = default;
};

/**
 * @class Minimax
 * @brief Minimax search algorithm.
 *
 * Searches the best move in a given game state using the minimax algorithm.
 * More here: https://en.wikipedia.org/wiki/Minimax
 *
 * @var useAlphaBeta Whether to use alpha-beta pruning
 * @var useCache Whether to use a cache
 * @var useTranspositionTable Whether to consider the cache results as final and return them
 * @var reorderMoves Whether to reorder moves
 * @var maxDepth The maximum depth of the search
 * @var moveTimeLimit The time limit for each move
 * @var evaluation The evaluation algorithm
 * @var cache The cache for storing best moves and scores
 * @var randomEngine Random engine used for non-deterministic moves
 */
class Minimax : public SearchAlgorithm {
private:
    bool useAlphaBeta;
    bool useCache;
    bool useTranspositionTable;
    bool reorderMoves;
    int maxDepth;
    long long moveTimeLimit; // Milliseconds
    Evaluation &evaluation;
    Cache<> cache; // Assuming template specialization if needed
    std::mt19937 randomEngine;

public:
    Minimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> minimax(Game &game, const Timer& timer, int leftDepth, long long alpha = INT32_MIN, long long beta = INT32_MIN);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
    void setMaxDepth(int newDepth);
};

/**
 * @class IterativeMinimax
 * @brief Iterative deepening minimax search algorithm.
 *
 * Searches the best move in a given game state using the minimax algorithm with iterative deepening.
 * More here: https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search
 *
 * @var minimax The minimax search algorithm
 * @var maxDepth The maximum depth of the search
 * @var moveTimeLimit The time limit for each move
 */
class IterativeMinimax : public SearchAlgorithm {
private:
    Minimax minimax;
    int maxDepth;
    long long moveTimeLimit;

public:
    IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &evaluation);
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
};

/**
 * @class RandomSearch
 * @brief Random search algorithm.
 *
 * Randomly chooses moves according to uniform distribution.
 */
class RandomSearch : public SearchAlgorithm {
public:
    RandomSearch();
    std::pair<int, piece_move> findBestMove(Game &game, const Timer& timer) override;
};

#endif // SEARCH_ALGORITHM_HPP
