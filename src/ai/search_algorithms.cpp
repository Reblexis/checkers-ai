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

std::pair<int, piece_move> Minimax::minimax(Board &board, int leftDepth, int alpha, int beta)
{
    curOperations++;

    bool maximizing = board.nextblack;
    int bestScore = maximizing ? INT32_MIN : INT32_MAX;
    piece_move bestMove = 0;

    if(useCache)
    {
        const cacheEntry &cacheInfo = cache.get(board);

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
        int score = evaluation.evaluate(board);
        if(useCache)
            cache.set(board, 1, score, 0);
        return {score, 0};
    }

    piece_moveList possibleMoves = board.moves();

    if(bestMove == 0)
    {
        bestMove = possibleMoves.begin()[0];
    }

    if(possibleMoves.size() == 0)
    {
        return {maximizing ? INT32_MIN : INT32_MAX, 0};
    }

    for(piece_move nextMove: possibleMoves)
    {
        if(curOperations >= operationLimit)
            break;

        board.play(nextMove);

        std::pair<int, piece_move> moveInfo = minimax(board, leftDepth-1, alpha, beta);

        if(maximizing ? piece_moveInfo.first > bestScore : moveInfo.first < bestScore)
        {
            bestScore = piece_moveInfo.first;
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

std::pair<int, piece_move> IterativeMinimax::findBestMove(Board &board)
{
    std::pair<int, piece_move> bestMove;
    bestMove.first = board.nextblack ? INT32_MIN : INT32_MAX;
    minimax.resetOperations();

    for(int i = 1; i <= maxDepth; i++)
    {
        std::pair<int, piece_move> candidate = minimax.findBestMove(board);
        if(candidate.second != 0)
            bestMove = candidate;

        if((bestMove.first==INT32_MAX && board.nextblack) || (bestMove.first == INT32_MIN && !board.nextblack) || candidate.second == 0)
            break;
    }

    if(bestMove.second==0)
        message("Invalid piece_move!", true, true, true);

    return bestMove;
}

RandomSearch::RandomSearch() {}

std::pair<int, piece_move> RandomSearch::findBestMove(Board &board)
{
    piece_moveList possibleMoves = board.moves();
    if(possibleMoves.size() == 0)
        return {board.nextblack ? INT32_MIN : INT32_MAX, 0};

    return {0, possibleMoves.begin()[rand64(board.hash) % possibleMoves.size()]};
}
