#include <string>

#include "../includes/constants.hpp"
#include "includes/agent.hpp"
#include "includes/evaluation.hpp"
#include "includes/hyperparameters.hpp"
#include "includes/search_algorithms.hpp"

Agent::Agent(const std::filesystem::path &dataPath): hyperparameters(dataPath / HYPERPARAMETERS_PATH) {
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

std::pair<int, piece_move> Agent::findBestMove(Game &game) {
    std::pair<int, piece_move> bestMove = searchAlgorithm->findBestMove(game);
    std::cout<<"Estimated score: "<<bestMove.first<<"\n";
    std::cout<<"Making move: "<<bestMove.second<<std::endl;
    return bestMove;
}

Agent::~Agent() {
    delete evaluation;
    delete searchAlgorithm;
}
