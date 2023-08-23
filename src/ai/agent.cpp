#include <string>

#include "../includes/constants.hpp"
#include "includes/hyperparameters.hpp"
#include "includes/search_algorithms.hpp"
#include "includes/evaluation.hpp"




class Agent{
private:
    Evaluation *evaluation;
    SearchAlgorithm *searchAlgorithm;
    Hyperparameters hyperparameters;

public:
   explicit Agent(const std::string &dataPath): hyperparameters(dataPath + HYPERPARAMETERS_PATH){
        if(hyperparameters.get<int>(EVALUATION_ALGORITHM_ID) == USE_ADVANCED_EVALUATION)
            evaluation = new AdvancedEvaluation(hyperparameters);
        else
            evaluation = new BasicEvaluation(hyperparameters);

        if(hyperparameters.get<int>(SEARCH_ALGORITHM_ID) == USE_MINIMAX)
            searchAlgorithm = new Minimax(hyperparameters, *evaluation);
        else if(hyperparameters.get<int>(SEARCH_ALGORITHM_ID) == USE_ITERATIVE_MINIMAX)
            searchAlgorithm = new IterativeMinimax(hyperparameters, *evaluation);
        else
            searchAlgorithm = new RandomSearch();
   }

    std::pair<int, move> findBestMove(Board &board){
         return searchAlgorithm->findBestMove(board);
    }
};