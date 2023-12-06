#include <random>

#include "includes/search_algorithms.hpp"
#include "../communication/includes/debugging.hpp"
#include "../communication/includes/game.hpp"

Minimax::Minimax(Hyperparameters &hyperparameters, Evaluation &eval)
        : evaluation(eval), cache()
{
    useAlphaBeta = hyperparameters.get<bool>(USE_ALPHA_BETA_ID);
    useCache = hyperparameters.get<bool>(USE_CACHE_ID);
    maxDepth = hyperparameters.get<int>(MAX_DEPTH_ID);
    operationLimit = hyperparameters.get<int>(OPERATION_LIMIT_ID);
}

std::pair<int, piece_move> Minimax::minimax(Game &game, int leftDepth, int alpha, int beta)
{
    curOperations++;
    const GameState& gameState = game.getGameState();

    bool maximizing = gameState.nextBlack;
    int bestScore = maximizing ? INT32_MIN : INT32_MAX;
    piece_move bestMove = 0;

    if(useCache)
    {
        const cacheEntry &cacheInfo = cache.get(gameState);

        if(leftDepth < cacheInfo.depth)
            return {cacheInfo.score, cacheInfo.bestMove};

        if(cacheInfo.depth!=0)
        {
            bestMove = cacheInfo.bestMove;
            bestScore = cacheInfo.score;
        }
    }

    if(leftDepth==0)
    {
        int score = evaluation.evaluate(gameState);
        if(useCache)
            cache.set(gameState, 1, score, 0);
        return {score, 0};
    }

    std::span<const piece_move> possibleMoves = gameState.getAvailableMoves();

    if(bestMove == 0)
    {
        bestMove = possibleMoves[0];
    }

    if(possibleMoves.size() == 0)
    {
        return {maximizing ? INT32_MIN : INT32_MAX, 0};
    }

    for(piece_move nextMove: possibleMoves)
    {
        if(curOperations >= operationLimit)
            break;

        game.makeMove(nextMove);

        std::pair<int, piece_move> moveInfo = minimax(board, leftDepth-1, alpha, beta);

        if(maximizing ? piece_moveInfo.first > bestScore : moveInfo.first < bestScore)
        {
            bestScore = piece_moveInfo.first;
            bestMove = nextMove;
        }

        game.undoMove();

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

    return {bestScore, bestMove};
}

std::pair<int, piece_move> Minimax::findBestMove(Board &b)
{
    resetOperations();
    return minimax(b, maxDepth);
}

void Minimax::resetOperations()
{
    curOperations = 0;
}

void Minimax::setMaxDepth(int newDepth)
{
    maxDepth = newDepth;
}

void Minimax::setOperationLimit(int newLimit)
{
    operationLimit = newLimit;
}

IterativeMinimax::IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &eval)
        : minimax(hyperparameters, eval)
{
    maxDepth = hyperparameters.get<int>(MAX_DEPTH_ID);
    operationLimit = hyperparameters.get<int>(OPERATION_LIMIT_ID);
}

std::pair<int, piece_move> IterativeMinimax::findBestMove(Game &game)
{
    const GameState& gameState = game.getGameState();

    std::pair<int, piece_move> bestMove;
    bestMove.first = gameState.nextblack ? INT32_MIN : INT32_MAX;
    minimax.resetOperations();

    for(int i = 1; i <= maxDepth; i++)
    {
        std::pair<int, piece_move> candidate = minimax.findBestMove(game);
        if(candidate.second != 0)
            bestMove = candidate;

        if((bestMove.first==INT32_MAX && gameState.nextblack) || (bestMove.first == INT32_MIN && !gameState.nextblack) || candidate.second == 0)
            break;
    }

    if(bestMove.second==0)
        message("Invalid piece_move!", true, true, true);

    return bestMove;
}

RandomSearch::RandomSearch() {}

std::pair<int, piece_move> RandomSearch::findBestMove(Game &game)
{
    const GameState& gameState = game.getGameState();
    std::span<piece_move> possibleMoves = game.getAvailableMoves();
    if(possibleMoves.size() == 0)
        return {gameState.nextblack ? INT32_MIN : INT32_MAX, 0};

    // Generate random move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, possibleMoves.size()-1);
    return {0, possibleMoves[distribution(gen)]};
}
