#include <map>
#include <string>
#include <vector>

#include "../../communication/includes/interface.hpp"

#include "../../includes/constants.hpp"

#include "hyperparameters.hpp"

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
    explicit BasicEvaluation(Hyperparameters &hyperparameters);
    int evaluate(const Board& board) override;
};

class AdvancedEvaluation : public Evaluation {
public:
    explicit AdvancedEvaluation(Hyperparameters &hyperparameters);

    int evaluate(Board &b);
};
