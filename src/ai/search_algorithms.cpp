#include <random>

#include "includes/search_algorithms.hpp"
#include "../communication/includes/debugging.hpp"
#include "../communication/includes/game.hpp"

Minimax::Minimax(Hyperparameters &hyperparameters, Evaluation &eval)
        : evaluation(eval), cache()
{
    useAlphaBeta = hyperparameters.get<bool>(USE_ALPHA_BETA_ID);
    useCache = hyperparameters.get<bool>(USE_CACHE_ID);
    reorderMoves = hyperparameters.get<bool>(REORDER_MOVES_ID);
    maxDepth = hyperparameters.get<int>(MAX_DEPTH_ID);
    operationLimit = INT32_MAX;
}

std::pair<int, piece_move> Minimax::minimax(Game &game, int leftDepth, int alpha, int beta)
{
    curOperations++;
    const GameState& gameState = game.getGameState();

    bool maximizing = gameState.nextBlack;
    int bestScore = maximizing ? INT32_MIN : INT32_MAX;
    piece_move bestMove = 0;

    if(leftDepth==0)
    {
        int score = evaluation.evaluate(gameState);
        return {score, 0};
    }

    std::span<const piece_move> possibleMovesSpan = gameState.getAvailableMoves();
    std::vector<piece_move> possibleMoves(possibleMovesSpan.begin(), possibleMovesSpan.end());

    if(reorderMoves){
        std::vector<std::pair<int, int>> scores(possibleMoves.size());
        for(int i = 0; i < possibleMoves.size(); i++){
            game.makeMove(possibleMoves[i]);
            scores[i] = {evaluation.evaluate(game.getGameState()), i};
            game.undoMove();
        }
        std::sort(scores.begin(), scores.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b){
            return (a.first > b.first);
        });
        if(!maximizing)
            std::reverse(scores.begin(), scores.end());
        std::vector<piece_move> newMoves(possibleMoves.size());
        for(int i = 0; i < possibleMoves.size(); i++)
            newMoves[i] = possibleMoves[scores[i].second];
        possibleMoves = newMoves;
    }

    if(useCache)
    {
        const cacheEntry &cacheInfo = cache.get(game.getGameState());
        bestMove = cacheInfo.bestMove;
        if(bestMove!=0){
            auto it = std::find(possibleMoves.begin(), possibleMoves.end(), bestMove);
            if(it != possibleMoves.end())
                std::iter_swap(possibleMoves.begin(), it);
        }
        //if(leftDepth < cacheInfo.depth)
          //  return {cacheInfo.score, cacheInfo.bestMove};
    }

    if(possibleMoves.empty())
    {
        return {maximizing ? INT32_MIN+1 : INT32_MAX-1, 0};
    }

    for(piece_move nextMove: possibleMoves)
    {
        if(curOperations >= operationLimit)
            break;

        game.makeMove(nextMove);

        std::pair<int, piece_move> moveInfo = minimax(game, leftDepth-1, alpha, beta);

        if(maximizing ? moveInfo.first > bestScore : moveInfo.first < bestScore)
        {
            bestScore = moveInfo.first;
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

    if(useCache)
        cache.set(gameState, leftDepth, bestScore, bestMove);

    if(curOperations >= operationLimit)
        return {maximizing ? INT32_MIN+1 : INT32_MAX-1, 0};

    return {bestScore, bestMove};
}

std::pair<int, piece_move> Minimax::findBestMove(Game &game)
{
    resetOperations();
    return minimax(game, maxDepth);
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
    minimax.setOperationLimit(operationLimit);
}

std::pair<int, piece_move> IterativeMinimax::findBestMove(Game &game)
{
    const GameState& gameState = game.getGameState();

    std::pair<int, piece_move> bestMove;
    bestMove.first = gameState.nextBlack ? INT32_MIN : INT32_MAX;
    minimax.resetOperations();

    for(int i = 1; i <= maxDepth; i++)
    {
        std::pair<int, piece_move> candidate = minimax.minimax(game, i);
        if(candidate.second != 0)
            bestMove = candidate;

        if((bestMove.first==INT32_MAX && gameState.nextBlack) || (bestMove.first == INT32_MIN && !gameState.nextBlack) || candidate.second == 0)
        {
            std::cout<<"Breaking at depth "<<i<<std::endl;
            break;
        }
    }

    return bestMove;
}

RandomSearch::RandomSearch() {}

std::pair<int, piece_move> RandomSearch::findBestMove(Game &game)
{
    const GameState& gameState = game.getGameState();
    std::span<const piece_move> possibleMoves = gameState.getAvailableMoves();
    if(possibleMoves.empty())
        return {gameState.nextBlack ? INT32_MIN : INT32_MAX, 0};

    // Generate random move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, possibleMoves.size()-1);
    return {0, possibleMoves[distribution(gen)]};
}
