#include <map>
#include <string>
#include <vector>

#include "../communication/includes/interface.hpp"

#include "../includes/constants.hpp"

#include "includes/hyperparameters.hpp"

class Evaluation {
    //Abstract class for evaluation classes
public:
    virtual int evaluate(const Board& board) = 0;
    virtual ~Evaluation() = default;
};

class BasicEvaluation : public Evaluation {
private:
    int pawnValue;
    int kingValue;

public:
    explicit BasicEvaluation(Hyperparameters &hyperparameters) : Evaluation()
    {
        pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
        kingValue = hyperparameters.get<int>(KING_VALUE_ID);
    }
    int evaluate(const Board& board) override {
        if(board.moves().size()==0)
            return board.nextblack ? INT32_MIN : INT32_MAX;
        return (board.blackPawnsCount() - board.whitePawnsCount()) * pawnValue + (board.blackKingsCount() - board.whiteKingsCount()) * kingValue;
    }
};

class AdvancedEvaluation : public Evaluation {
private:
    int pawnValue;
    int kingValue;
    int diffMultiplier;
    std::vector<int> kingTable = std::vector<int>(NUM_SQUARES);
    std::vector<int> pawnTable = std::vector<int>(NUM_SQUARES);
public:
    explicit AdvancedEvaluation(Hyperparameters &hyperparameters) : Evaluation()
    {
        pawnValue = hyperparameters.get<int>(PAWN_VALUE_ID);
        kingValue = hyperparameters.get<int>(KING_VALUE_ID);
        diffMultiplier = hyperparameters.get<int>(DIFF_MULTIPLIER_ID);
        kingTable = hyperparameters.get<std::vector<int>>(KING_TABLE_ID);
        pawnTable = hyperparameters.get<std::vector<int>>(PAWN_TABLE_ID);
    }

    int evaluate(Board &b){
        if (b.moves().size() == 0)
            return b.nextblack ? INT32_MIN : INT32_MAX;

        int score = 0;
        int pawnTableScore = 0;
        int kingTableScore = 0;

        // Add to pawnTableScore score for pawns at the positions specified in the pawn table
        // Black has the table flipped
        for(int i = 0; i < NUM_SQUARES; i++)
        {
            if(b.b & (1 << i))
                pawnTableScore += pawnTable[NUM_SQUARES - i - 1];
            else if(b.w & (1 << i))
                pawnTableScore -= pawnTable[i];
        }

        // Add score for kings at the positions specified in the king table
        for(int i = 0; i < NUM_SQUARES; i++)
        {
            if(b.bk & (1 << i))
                kingTableScore += kingTable[NUM_SQUARES - i - 1];
            else if(b.wk & (1 << i))
                kingTableScore -= kingTable[i];
        }

        // Add score for the difference
        int pawnDiff = b.blackPawnsCount() - b.whitePawnsCount();
        int kingDiff = b.blackKingsCount() - b.whiteKingsCount();

        pawnDiff *= pawnValue * diffMultiplier;
        kingDiff *= kingValue * diffMultiplier;
        kingTableScore *= kingValue;
        pawnTableScore *= pawnValue;

        score = pawnDiff + kingDiff + kingTableScore + pawnTableScore;
        return score;
    }
};