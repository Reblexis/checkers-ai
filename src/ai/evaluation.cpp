#include <map>
#include <string>

#include "../communication/includes/interface.hpp"

#include "../constants.hpp"

class Evaluation {
    //Abstract class for evaluation classes
public:
    virtual int evaluate(const Board& board) = 0;
    virtual ~Evaluation() = default;
};

class BasicEvaluation : public Evaluation {
private:
    const std::string PAWN_VALUE_ID = "pawn_value";
    const std::string KING_VALUE_ID = "king_value";

    int pawnValue;
    int kingValue;

public:
    explicit BasicEvaluation(const std::map<std::string, int>& hyperparameters) : Evaluation()
    {
        pawnValue = hyperparameters.at(PAWN_VALUE_ID);
        kingValue = hyperparameters.at(KING_VALUE_ID);
    }
    int evaluate(const Board& board) override {
        if(board.moves().size()==0)
            return board.nextblack ? INT32_MIN : INT32_MAX;
        return (board.blackPawnsCount() - board.whitePawnsCount()) * pawnValue + (board.blackKingsCount() - board.whiteKingsCount()) * kingValue;
    }
};

class AdvancedEvaluation : public Evaluation {
private:
    const std::string PAWN_VALUE_ID = "pawn_value";
    const std::string KING_VALUE_ID = "king_value";
    const std::string DIFF_MULTIPLIER_ID = "diff_multiplier";
    const std::string KING_TABLE_ID = "king_table_";
    const std::string PAWN_TABLE_ID = "pawn_table_";

    int pawnValue;
    int kingValue;
    int diffMultiplier;
    int kingTable[NUM_SQUARES];
    int pawnTable[NUM_SQUARES];

};