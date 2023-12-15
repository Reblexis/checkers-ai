#include <random>

#include "includes/tournament.hpp"

Tournament::Tournament(){};

void Tournament::simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game){
    unsigned int moves = 0;
    message("Starting new game!", true);
    std::cout<<"White rating: "<<whiteAgent->getRating()<<"\n";
    std::cout<<"Black rating: "<<blackAgent->getRating()<<"\n";
    std::cout<<game.getGameState().board<<"\n";

    while(!game.isFinished() && moves < MAX_MOVES){
        bool nextBlack = game.getGameState().nextBlack;
        std::pair<int, piece_move> receivedMove = nextBlack ? blackAgent->findBestMove(game) : whiteAgent->findBestMove(game);
        game.makeMove(receivedMove.second);
        moves++;
        std::cout<<game.getGameState().board<<"\n";
    }

    int whiteRating = whiteAgent->getRating();
    int blackRating = blackAgent->getRating();
    double result = game.getGameState().nextBlack ? 1 : 0;
    if(game.isFinished()){
        result = 0.5;
    }

    whiteAgent->addGame(blackRating, result);
    blackAgent->addGame(whiteRating,  1 - result);

    std::cout<<"Game finished! Result: "<<result<<"\n";
    std::cout<<"White rating: "<<whiteAgent->getRating()<<"\n";
    std::cout<<"Black rating: "<<blackAgent->getRating()<<"\n";
}

void Tournament::simulateMatch(Agent *agent1, Agent *agent2, int randomMovesCount){
    Game game{};
    RandomSearch randomSearch;
    for(int i = 0; i < randomMovesCount; i++){
        game.makeMove(randomSearch.findBestMove(game).second);
    }
    Game game2 = game;

    simulateGame(agent1, agent2, game);
    simulateGame(agent2, agent1, game2);
}

void Tournament::randomMatches(std::vector<Agent*> &agents, int matches, int randomMovesCount = 6){
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, agents.size() - 1);

    for(int i = 0; i < matches; i++){
        message("Starting match " + std::to_string(i + 1) + " of " + std::to_string(matches) + "!", true);

        int agent1Index = dis(g);
        int agent2Index = dis(g);
        while(agent1Index == agent2Index){
            agent2Index = dis(g);
        }
        Agent *agent1 = agents[agent1Index];
        Agent *agent2 = agents[agent2Index];

        simulateMatch(agent1, agent2, randomMovesCount);
    }
}


