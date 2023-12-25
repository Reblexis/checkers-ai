#include <random>
#include <utility>

#include "includes/tournament.hpp"

Tournament::Tournament(std::string id, bool visualize) : id(std::move(id)), visualize(visualize)
{
    std::filesystem::create_directories(TOURNAMENT_LOGS_PATH / id);
}

void Tournament::simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const{
    unsigned int moves = 0;
    Statistics whiteStatistics = Statistics(TOURNAMENT_LOGS_PATH / id / (whiteAgent->id + ".json"));
    Statistics blackStatistics = Statistics(TOURNAMENT_LOGS_PATH / id / (blackAgent->id + ".json"));
    message("Starting new game!", true);
    std::cout<<"White rating: "<<whiteStatistics.getRating()<<"\n";
    std::cout<<"Black rating: "<<blackStatistics.getRating()<<"\n";
    App app;
    if(visualize){
        app.launch();
        app.refreshWindow(game, UI({}, {}));
    }

    while(!game.isFinished() && moves < MAX_MOVES){
        bool nextBlack = game.getGameState().nextBlack;
        std::pair<int, piece_move> receivedMove = nextBlack ? blackAgent->findBestMove(game) : whiteAgent->findBestMove(game);
        game.makeMove(receivedMove.second);
        moves++;
        if(visualize){
            app.refreshWindow(game, UI({}, game.getGameState().getMove(receivedMove.second).path));
        }
    }

    int whiteRating = whiteStatistics.getRating();
    int blackRating = blackStatistics.getRating();
    double result = game.getGameState().nextBlack ? 1 : 0;
    if(moves >= MAX_MOVES){
        result = 0.5;
    }

    whiteStatistics.addGame(blackRating, result);
    blackStatistics.addGame(whiteRating,  1 - result);


    std::cout<<"Game finished! Result: "<<result<<"\n";
    std::cout<<"White rating: "<<whiteStatistics.getRating()<<"\n";
    std::cout<<"Black rating: "<<blackStatistics.getRating()<<"\n";
}

void Tournament::simulateMatch(Agent *agent1, Agent *agent2, int randomMovesCount) const{
    Game game{};
    RandomSearch randomSearch;
    for(int i = 0; i < randomMovesCount; i++){
        game.makeMove(randomSearch.findBestMove(game).second);
    }
    Game game2 = game;

    simulateGame(agent1, agent2, game);
    simulateGame(agent2, agent1, game2);
}

void Tournament::randomMatches(std::vector<Agent*> &agents, int matches, int randomMovesCount = 6, std::optional<int> focusAgent) const{
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, agents.size() - 1);

    for(int i = 0; i < matches; i++){
        message("Starting match " + std::to_string(i + 1) + " of " + std::to_string(matches) + "!", true);

        int agent1Index = focusAgent.value_or(dis(g));
        int agent2Index = dis(g);
        while(agent1Index == agent2Index){
            agent2Index = dis(g);
        }
        Agent *agent1 = agents[agent1Index];
        Agent *agent2 = agents[agent2Index];

        simulateMatch(agent1, agent2, randomMovesCount);
    }
}


