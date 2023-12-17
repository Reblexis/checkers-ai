#include "includes/evaluation.hpp"
#include "../communication/includes/game.hpp"
#include "../includes/constants.hpp"
#include "includes/hyperparameters.hpp"

BasicEvaluation::BasicEvaluation(Hyperparameters &hyperparameters)
{
    pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
    kingValue = hyperparameters.get<int>(KING_VALUE_ID);
}

int BasicEvaluation::evaluate(const GameState& gameState)
{
    const Board& board = gameState.board;
    if(gameState.getAvailableMoves().empty())
        return gameState.nextBlack ? INT32_MIN : INT32_MAX;
    return (board.blackPawnsCount() - board.whitePawnsCount()) * pawnValue + (board.blackKingsCount() - board.whiteKingsCount()) * kingValue;
}

AdvancedEvaluation::AdvancedEvaluation(Hyperparameters &hyperparameters)
{
    pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
    kingValue = hyperparameters.get<int>(KING_VALUE_ID);
    diffMultiplier = hyperparameters.get<int>(DIFF_MULTIPLIER_ID);
    kingTable = hyperparameters.get<std::vector<int>>(KING_TABLE_ID);
    pawnTable = hyperparameters.get<std::vector<int>>(PAWN_TABLE_ID);
}

int AdvancedEvaluation::evaluate(const GameState &gameState)
{
    if (gameState.getAvailableMoves().empty())
        return gameState.nextBlack ? INT32_MIN : INT32_MAX;

    int score = 0;
    int pawnTableScore = 0;
    int kingTableScore = 0;
    Board board = gameState.board;

    bitboard whitePawns = board.getWhitePieces() & ~board.getWhiteKings();
    bitboard blackPawns = board.getBlackPieces() & ~board.getBlackKings();


    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(whitePawns & (1 << i))
            pawnTableScore -= pawnTable[i];
        else if(blackPawns & (1 << i))
            pawnTableScore += pawnTable[NUM_SQUARES - i - 1];
    }

    bitboard whiteKings = board.getWhiteKings();
    bitboard blackKings = board.getBlackKings();
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(whiteKings & (1 << i))
            kingTableScore -= kingTable[i];
        else if(blackKings & (1 << i))
            kingTableScore += kingTable[NUM_SQUARES - i - 1];
    }

    float pawnDiff = static_cast<float>(board.blackPawnsCount() - board.whitePawnsCount());
    float kingDiff = static_cast<float>(board.blackKingsCount() - board.whiteKingsCount());

    pawnDiff *= 100.0f / static_cast<float>(board.allPiecesCount()) * pawnValue * diffMultiplier ;
    kingDiff *= 100.0f / static_cast<float>(board.allPiecesCount()) * kingValue * diffMultiplier;
    kingTableScore *= kingValue;
    pawnTableScore *= pawnValue;

    score = static_cast<int>(pawnDiff + kingDiff) + kingTableScore + pawnTableScore;
    return score;
}

