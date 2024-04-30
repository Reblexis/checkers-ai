#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <map>
#include <string>
#include <vector>

#include "../../communication/includes/game.hpp"

#include "../../includes/constants.hpp"

#include "hyperparameters.hpp"

/**
 * @class Evaluation
 * @brief Abstract class for evaluation classes.
 *
 * Serves for zero-step evaluation of the game state.
 */
class Evaluation {
    //Abstract class for evaluation classes
public:
    virtual int evaluate(const GameState& gameState) = 0; ///< Returns an evaluation score for the given game state from the perspective of the current player.
    virtual ~Evaluation() = default;
};

/**
 * @class BasicEvaluation
 * @brief Basic evaluation class.
 *
 * Evaluates the game state based on the number of pawns and kings.
 *
 * @var pawnValue The value of a pawn
 * @var kingValue The value of a king
 */
class BasicEvaluation : public Evaluation {
private:
    int pawnValue;
    int kingValue;

public:
    explicit BasicEvaluation(Hyperparameters &hyperparameters);
    int evaluate(const GameState& gameState) override;
};

/**
 * @class AdvancedEvaluation
 * @brief Advanced evaluation class.
 *
 * Evaluates the game state based on the number of pawns and kings and their positions.
 *
 * @var pawnValue The value of a pawn
 * @var kingValue The value of a king
 * @var diffMultiplier The multiplier for the difference in the number of pawns and kings
 * @var kingTable The table of king values
 * @var pawnTable The table of pawn values
 */
class AdvancedEvaluation : public Evaluation {
private:
    int pawnValue;
    int kingValue;
    int diffMultiplier;
    std::vector<int> kingTable;
    std::vector<int> pawnTable;

public:
    explicit AdvancedEvaluation(Hyperparameters &hyperparameters);
    int evaluate(const GameState& gameState) override;
};

#endif