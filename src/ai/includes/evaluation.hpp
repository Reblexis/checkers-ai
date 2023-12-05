#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include <map>
#include <string>
#include <vector>

#include "../../communication/includes/game.hpp"

#include "../../includes/constants.hpp"

#include "hyperparameters.hpp"

class Evaluation {
    //Abstract class for evaluation classes
public:
    virtual int evaluate(const GameState& gameState) = 0;
    virtual ~Evaluation() = default;
};

class BasicEvaluation : public Evaluation {
private:
    int pawnValue;
    int kingValue;

public:
    explicit BasicEvaluation(Hyperparameters &hyperparameters);
    int evaluate(const GameState& gameState) override;
};

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