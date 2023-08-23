#include "../communication/includes/debugging.hpp"
#include "../communication/includes/interface.hpp"

#include "includes/hyperparameters.hpp"
#include "includes/cache.hpp"
#include "includes/evaluation.hpp"


// This is responsible for doing the search algorithms and evaluation using the given interface
// for the game checkers

class SearchAlgorithm{
    public:
    virtual std::pair<int, move> findBestMove(Board &b) = 0;
    virtual ~SearchAlgorithm() = default;
};

class Minimax: public SearchAlgorithm{
private:
    bool useAlphaBeta;
    bool useCache;
    int maxDepth;
    int operationLimit;

    int curOperations = 0;

    Evaluation &evaluation;
    Cache<> cache;
public:
    Minimax(Hyperparameters &hyperparameters, Evaluation &evaluation) : SearchAlgorithm(), cache(), evaluation(evaluation)
    {
        useAlphaBeta = hyperparameters.get<bool>(USE_ALPHA_BETA_ID);
        useCache = hyperparameters.get<bool>(USE_CACHE_ID);
        maxDepth = hyperparameters.get<int>(MAX_DEPTH_ID);
        operationLimit = hyperparameters.get<int>(OPERATION_LIMIT_ID);

        this->evaluation = evaluation;
    }

    std::pair<int, move> minimax(Board &board, int leftDepth, int alpha = INT32_MIN, int beta = INT32_MAX)
    {
        curOperations++;

        bool maximizing = board.nextblack;
        int bestScore = maximizing ? INT32_MIN : INT32_MAX;
        move bestMove = 0;

        if(useCache)
        {
            const cacheEntry &cacheInfo = cache.get(board);

            if(leftDepth < cacheInfo.depth)
                return {cacheInfo.score, cacheInfo.bestMove};

            if(cacheInfo.depth!=0)
            {
                // Choose the best entry saved to improve speed of search with alpha-beta
                bestMove = cacheInfo.bestMove;
                bestScore = cacheInfo.score;
            }
        }

        if(leftDepth==0)
        {
            // Evaluate at leaf nodes
            int score = evaluate(board);
            if(useCache)
                cache.set(board, 1, score, 0);
            return {score, 0};
        }

        moveList possibleMoves = board.moves();

        if(possibleMoves.size() == 0)
        {
            // If there are no moves, return the worst possible score
            return {maximizing ? INT32_MIN : INT32_MAX, 0};
        }

        for(move nextMove: possibleMoves)
        {
            if(curOperations >= operationLimit)
                break;

            board.play(nextMove);

            std::pair<int, move> moveInfo = minimax(board, leftDepth-1, alpha, beta);

            if(maximizing ? moveInfo.first > bestScore : moveInfo.first < bestScore)
            {
                // If we have found a better move, update the best move
                bestScore = moveInfo.first;
                bestMove = nextMove;
            }

            board.undo();

            if(useAlphaBeta)
            {
                if(maximizing)
                    alpha = std::max(alpha, bestScore);
                else
                    beta = std::min(beta, bestScore);

                if(beta <= alpha)
                    break;
            }
        }

        if(curOperations >= operationLimit)
            return {0, 0};
    }

    std::pair<int, move> findBestMove(Board &b) override
    {
        resetOperations();
        return minimax(b, maxDepth);
    }

    void resetOperations()
    {
        curOperations = 0;
    }

    void setMaxDepth(int newDepth){
        maxDepth = newDepth;
    }

    void setOperationLimit(int newLimit){
        operationLimit = newLimit;
    }
};

class IterativeMinimax: public SearchAlgorithm{
private:
    Minimax minimax;
    int maxDepth;
    int operationLimit;

public:
    IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &evaluation) : SearchAlgorithm(), minimax(hyperparameters, evaluation)
    {
        maxDepth = hyperparameters.get<int>(MAX_DEPTH_ID);
        operationLimit = hyperparameters.get<int>(OPERATION_LIMIT_ID);
    }

    std::pair<int, move> findBestMove(Board &board) override
    {
        std::pair<int, move> bestMove;
        bestMove.first = board.nextblack ? INT32_MIN : INT32_MAX;
        minimax.resetOperations();

        for(int i = 1; i <= maxDepth; i++)
        {
            std::pair<int, move> candidate = minimax.findBestMove(board);
            if(candidate.second != 0)
                bestMove = candidate;

            if((bestMove.first==INT32_MAX && board.nextblack) || (bestMove.first == INT32_MIN && !board.nextblack) || candidate.second == 0)
                break;
        }

        if(bestMove.second==0)
            message("Invalid move!", true, true, true);

        return bestMove;
    }
};

class RandomSearch: public SearchAlgorithm{
public:
    RandomSearch() : SearchAlgorithm(){}

    std::pair<int, move> findBestMove(Board &board) override
    {
        moveList possibleMoves = board.moves();
        if(possibleMoves.size() == 0)
            return {board.nextblack ? INT32_MIN : INT32_MAX, 0};

        return {0, possibleMoves.begin()[rand64(board.hash) % possibleMoves.size()]};
    }
};