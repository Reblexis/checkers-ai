#include <random>

#include "includes/search_algorithms.hpp"
#include "../communication/includes/debugging.hpp"
#include "../communication/includes/game.hpp"

Minimax::Minimax(Hyperparameters &hyperparameters, Evaluation &eval)
        : evaluation(eval), cache(), maxDepth(hyperparameters.get<int>(MAX_DEPTH_ID)), useAlphaBeta(hyperparameters.get<bool>(USE_ALPHA_BETA_ID)),
        useCache(hyperparameters.get<bool>(USE_CACHE_ID)), useTranspositionTable(hyperparameters.get<bool>(USE_TRANSPOSITION_TABLE_ID)),
        reorderMoves(hyperparameters.get<bool>(REORDER_MOVES_ID)),
        moveTimeLimit(hyperparameters.get<long long>(MOVE_TIME_LIMIT_ID)), randomEngine(std::random_device()())
{
}

/**
 * @brief Minimax search algorithm.
 * @param game The game manager class allowing for game state manipulation and available moves retrieval.
 * @param timer Allows for termination of the search algorithm after a specified time limit.
 * @param leftDepth The remaining depth of the search.
 * @param alpha The alpha value for alpha-beta pruning from the perspective of the alpha player.
 * @param beta The beta value for alpha-beta pruning from the perspective of the beta player.
 * @return The best move found by the search algorithm and its score.
 */
std::pair<int, piece_move> Minimax::minimax(Game &game, const Timer& timer, int leftDepth, long long alpha, long long beta)
{
    const GameState& gameState = game.getGameState();

    long long bestScore = INT32_MIN;
    piece_move bestMove = 0;
    // Upper bound and lower bound of the possible score for the current state
    long long upperBound = (gameState.nextBlack ? -beta : -alpha); // Negate beta and alpha because of the different perspective of the other player
    long long lowerBound = (gameState.nextBlack ? alpha : beta);

    if(leftDepth==0) // Leaf node
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
        std::vector<piece_move> newMoves(possibleMoves.size());
        for(int i = 0; i < possibleMoves.size(); i++)
            newMoves[i] = possibleMoves[scores[i].second];
        possibleMoves = newMoves;
    }

    // Shuffle for random move selection
    shuffle(possibleMoves.begin(), possibleMoves.end(), randomEngine);

    if(useCache) {
        const CacheEntry &cacheInfo = cache.get(game.getGameState());
        bestMove = cacheInfo.bestMove;
        if (bestMove != 0) {
            // Move the best found move from the cache to the front of the vector for more efficient search (using alpha-beta pruning)
            auto it = std::find(possibleMoves.begin(), possibleMoves.end(), bestMove);
            if (it != possibleMoves.end())
                std::iter_swap(possibleMoves.begin(), it);
        }
        if (useTranspositionTable && leftDepth == cacheInfo.depth && upperBound == cacheInfo.upperBound && lowerBound == cacheInfo.lowerBound) // Retrieve only if the search parameters match
        {
            return {cacheInfo.score, cacheInfo.bestMove};
        }
    }

    if(possibleMoves.empty()) // No moves available = loss
    {
        return {INT32_MIN+1, 0};
    }

    for(piece_move nextMove: possibleMoves)
    {
        if(timer.isFinished())
            break;

        game.makeMove(nextMove);

        std::pair<int, piece_move> moveInfo = minimax(game, timer, leftDepth-1, alpha, beta);
        moveInfo.first *= -1; // Align to the current player's perspective

        if(moveInfo.first > bestScore)
        {
            bestScore = moveInfo.first;
            bestMove = nextMove;
        }

        game.undoMove();

        if(useAlphaBeta)
        {
            if(gameState.nextBlack)
                alpha = std::max(alpha, bestScore);
            else
                beta = std::max(beta, bestScore);

            if(-beta <= alpha) // Prune whilst counting with the different perspective
                break;
        }
    }


    if(timer.isFinished()) // If the search was interrupted, return the best move found so far
        return {INT32_MIN+1, bestMove};

    if(useCache)
        cache.set(gameState, leftDepth, bestScore, upperBound, lowerBound, bestMove);

    return {bestScore, bestMove};
}

std::pair<int, piece_move> Minimax::findBestMove(Game &game, const Timer& timer)
{
    Timer localTimer = Timer(std::min(moveTimeLimit, timer.getRemainingTime()));
    return minimax(game, localTimer, maxDepth);
}

void Minimax::setMaxDepth(int newDepth)
{
    maxDepth = newDepth;
}

IterativeMinimax::IterativeMinimax(Hyperparameters &hyperparameters, Evaluation &eval)
        : minimax(hyperparameters, eval), maxDepth(hyperparameters.get<int>(MAX_DEPTH_ID)),
        moveTimeLimit(hyperparameters.get<long long>(MOVE_TIME_LIMIT_ID))
{
}

/**
 * @brief Iterative deepening minimax search algorithm.
 * @param game The game manager class allowing for game state manipulation and available moves retrieval.
 * @param timer Allows for termination of the search algorithm after a specified time limit.
 * @return The best move found by the search algorithm and its score.
 */
std::pair<int, piece_move> IterativeMinimax::findBestMove(Game &game, const Timer& timer)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    const GameState& gameState = game.getGameState();

    std::pair<int, piece_move> bestMove;
    bestMove.first = INT32_MIN;
    bestMove.second = gameState.getAvailableMoves()[0];

    Timer localTimer = Timer(std::min(moveTimeLimit, timer.getRemainingTime()/4));
    localTimer.resume();

    for(int i = 1; i <= maxDepth; ++i)
    {
        std::pair<int, piece_move> candidate = minimax.minimax(game, localTimer, i);
        if(candidate.second != 0 && !localTimer.isFinished())
            bestMove = candidate;

        if(bestMove.first==INT32_MAX || candidate.second == 0 || localTimer.isFinished()) // If winning path was found or the search was interrupted, end
        {
            break;
        }
    }

    return bestMove;
}

RandomSearch::RandomSearch() = default;

std::pair<int, piece_move> RandomSearch::findBestMove(Game &game, const Timer& timer)
{
    const GameState& gameState = game.getGameState();
    std::span<const piece_move> possibleMoves = gameState.getAvailableMoves();
    if(possibleMoves.empty())
        return {INT32_MIN, 0};

    // Generate random move
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, possibleMoves.size()-1);
    return {0, possibleMoves[distribution(gen)]};
}
