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
    if(gameState.getAvailableMoves().size())
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
    if (gameState.getAvailableMoves().size() == 0)
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
            pawnTableScore += pawnTable[NUM_SQUARES - i - 1];
        else if(blackPawns & (1 << i))
            pawnTableScore -= pawnTable[i];
    }

    bitboard whiteKings = board.getWhiteKings();
    bitboard blackKings = board.getBlackKings();
    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(whiteKings & (1 << i))
            kingTableScore += kingTable[NUM_SQUARES - i - 1];
        else if(blackKings & (1 << i))
            kingTableScore -= kingTable[i];
    }

    int pawnDiff = board.blackPawnsCount() - board.whitePawnsCount();
    int kingDiff = board.blackKingsCount() - board.whiteKingsCount();

    pawnDiff *= pawnValue * diffMultiplier;
    kingDiff *= kingValue * diffMultiplier;
    kingTableScore *= kingValue;
    pawnTableScore *= pawnValue;

    score = pawnDiff + kingDiff + kingTableScore + pawnTableScore;
    return score;
}
