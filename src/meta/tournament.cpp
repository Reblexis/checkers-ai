#include <random>
#include <utility>
#include <chrono>
#include <format>

#include "includes/tournament.hpp"
#include "includes/timer.hpp"

Tournament::Tournament(std::string id, bool visualize, int timeLimit, int randomStartMoves) : id(std::move(id)), visualize(visualize), timeLimit(timeLimit), randomStartMoves(randomStartMoves)
{
    std::filesystem::create_directories(TOURNAMENT_LOGS_PATH / id);
}

void Tournament::simulateGame(Agent *whiteAgent, Agent *blackAgent, Game &game) const{
    if(whiteAgent->id == blackAgent->id){
        throw std::runtime_error(std::format("Opposing agents have the same id: {}", whiteAgent->id));
    }

    unsigned int moves = 0;
    Statistics whiteStatistics = Statistics(TOURNAMENT_LOGS_PATH / id / (whiteAgent->id + ".json"));
    Statistics blackStatistics = Statistics(TOURNAMENT_LOGS_PATH / id / (blackAgent->id + ".json"));
    message("Starting new game!", true);
    std::cout<<std::format("{} (white, rating: {}) vs {} (black, rating: {})\n", whiteAgent->id, whiteStatistics.getRating(), blackAgent->id, blackStatistics.getRating());

    Timer timerWhite(timeLimit * 1000);
    Timer timerBlack(timeLimit * 1000);
    timerWhite.isFinished();
    timerBlack.isFinished();

    App app;
    if(visualize){
        app.launch();
        app.refreshWindow(game, UI({}, {}));
    }

    while(!game.isFinished() && moves < MAX_MOVES){
        bool nextBlack = game.getGameState().nextBlack;

        Timer *timer = nextBlack ? &timerBlack : &timerWhite;
        timer->resume();
        std::pair<int, piece_move> receivedMove = nextBlack ? blackAgent->findBestMove(game, *timer) : whiteAgent->findBestMove(game, *timer);
        timer->pause();

        std::cout<< std::format("White agent (id: {}) has {} ms left.\n", whiteAgent->id, timerWhite.getRemainingTime());
        std::cout<< std::format("Black agent (id: {}) has {} ms left.\n", blackAgent->id, timerBlack.getRemainingTime());
        if(timer->isFinished()){
            std::cout<<"Time limit exceeded!\n";
            break;
        }

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

void Tournament::simulateMatch(Agent *agent1, Agent *agent2) const{
    Game game{};
    RandomSearch randomSearch;
    for(int i = 0; i < randomStartMoves; i++){
        game.makeMove(randomSearch.findBestMove(game, Timer(0)).second);
    }
    Game game2 = game;

    simulateGame(agent1, agent2, game);
    simulateGame(agent2, agent1, game2);
}

void Tournament::randomMatches(std::vector<Agent*> &agents, int matches, std::optional<int> focusAgent) const{
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

        simulateMatch(agent1, agent2);
    }
}


