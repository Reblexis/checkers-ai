#include "includes/evaluation.hpp"
#include "../communication/includes/interface.hpp"
#include "../includes/constants.hpp"
#include "includes/hyperparameters.hpp"

// Implementation for BasicEvaluation class

BasicEvaluation::BasicEvaluation(Hyperparameters &hyperparameters)
{
    pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
    kingValue = hyperparameters.get<int>(KING_VALUE_ID);
}

int BasicEvaluation::evaluate(const Board& board)
{
    if(board.moves().size()==0)
        return board.nextblack ? INT32_MIN : INT32_MAX;
    return (board.blackPawnsCount() - board.whitePawnsCount()) * pawnValue + (board.blackKingsCount() - board.whiteKingsCount()) * kingValue;
}

// Implementation for AdvancedEvaluation class

AdvancedEvaluation::AdvancedEvaluation(Hyperparameters &hyperparameters)
{
    pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
    kingValue = hyperparameters.get<int>(KING_VALUE_ID);
    diffMultiplier = hyperparameters.get<int>(DIFF_MULTIPLIER_ID);
    kingTable = hyperparameters.get<std::vector<int>>(KING_TABLE_ID);
    pawnTable = hyperparameters.get<std::vector<int>>(PAWN_TABLE_ID);
}

int AdvancedEvaluation::evaluate(const Board &board)
{
    if (board.moves().size() == 0)
        return board.nextblack ? INT32_MIN : INT32_MAX;

    int score = 0;
    int pawnTableScore = 0;
    int kingTableScore = 0;

    bitboard whitePawns = board.whitePieces & ~board.whiteKings;
    bitboard blackPawns = board.blackPieces & ~board.blackKings;

    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(whitePawns & (1 << i))
            pawnTableScore += pawnTable[NUM_SQUARES - i - 1];
        else if(blackPawns & (1 << i))
            pawnTableScore -= pawnTable[i];
    }

    for(int i = 0; i < NUM_SQUARES; i++)
    {
        if(board.blackKings & (1 << i))
            kingTableScore += kingTable[NUM_SQUARES - i - 1];
        else if(board.whiteKings & (1 << i))
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
