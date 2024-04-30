#include <random>
#include <utility>
#include <chrono>
#include <format>

#include "includes/tournament.hpp"
#include "includes/timer.hpp"

Tournament::Tournament(std::string id, std::vector<std::unique_ptr<Agent>> &&agents, TournamentType tournamentType, bool visualize, int timeLimit, int maxMoves): id(std::move(id)), agents(std::move(agents)), tournamentType(tournamentType), visualize(visualize), timeLimit(timeLimit), maxMoves(maxMoves)
{
    std::filesystem::create_directories(TOURNAMENT_LOGS_PATH / id);

    if (this->agents.size() <= 1) {
        throw std::runtime_error(std::format("Tournament {} has less than 2 agents! Agent count: {}", id, agents.size()));
    }
    if (timeLimit <= 0) {
        throw std::runtime_error(std::format("Tournament {} has invalid time limit: {}", id, timeLimit));
    }

    launch();
}

Tournament Tournament::createFromFile(const std::filesystem::path &path){
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(std::format("Tournament config file {} not found.", path.string()));
    }

    std::ifstream
    file(path);
    nlohmann::json json;
    file>>json;
    std::vector<std::unique_ptr<Agent>> agents;
    for(const auto &agentJson : json["agents"]){
        std::string type = agentJson["type"];
        if(type == "hyperparameters"){
            agents.push_back(std::make_unique<HyperparametersAgent>(agentJson["path"], agentJson["id"]));
        } else if(type == "executable"){
            agents.push_back(std::make_unique<ExecutableAgent>(agentJson["path"], agentJson["id"]));
        }
        else if (type=="player") {
            agents.push_back(std::make_unique<Player>(agentJson["id"]));
        }
        else{
            throw std::runtime_error(std::format("Unknown agent type: {}", type));
        }
    }

    if(!TOURNAMENT_TYPE_MAP.contains(json["tournamentType"])){
        throw std::runtime_error(std::format("Unknown tournament type: {}", json["tournamentType"].dump()));
    }
    return Tournament(json["id"], std::move(agents), TOURNAMENT_TYPE_MAP.at(json["tournamentType"]), json["visualize"], json["timeLimit"], json["maxMoves"]);
}

void Tournament::launch(){
    if(tournamentType == TournamentType::roundRobin){
        roundRobin();
    } else if(tournamentType == TournamentType::randomMatches){
        randomMatches();
    }
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

    Timer timerWhite(timeLimit);
    Timer timerBlack(timeLimit);

    whiteAgent->initialize(timeLimit, false);
    blackAgent->initialize(timeLimit, true);

    // TODO: fix errors related to multiple X11 calls (XInitThreads)
    // When agent initialization is moved below this, errors start appearing
    App app;
    if(visualize){
        app.launch("visualizer");
        app.refresh(game);
    }


    while(!game.isFinished() && moves < maxMoves){
        bool nextBlack = game.getGameState().nextBlack;

        Timer *timer = nextBlack ? &timerBlack : &timerWhite;
        timer->resume();
        std::pair<int, piece_move> receivedMove = nextBlack ? blackAgent->findBestMove(game, *timer) : whiteAgent->findBestMove(game, *timer);
        timer->pause();

        // std::cout<< std::format("White agent (id: {}) has {} ms left.\n", whiteAgent->id, timerWhite.getRemainingTime());
        // std::cout<< std::format("Black agent (id: {}) has {} ms left.\n", blackAgent->id, timerBlack.getRemainingTime());
        if(timer->isFinished()){
            std::cout<<"Time limit exceeded!\n";
            break;
        }

        game.makeMove(receivedMove.second);
        moves++;
        if(visualize){
            app.refresh(game);
        }
    }

    whiteAgent->die();
    blackAgent->die();

    int whiteRating = whiteStatistics.getRating();
    int blackRating = blackStatistics.getRating();
    double result = game.getGameState().nextBlack ? 1 : 0;
    if(moves >= maxMoves){
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
    Game game2 = game;

    simulateGame(agent1, agent2, game);
    simulateGame(agent2, agent1, game2);
}

void Tournament::randomMatches(int matches, std::optional<int> focusAgent) const{
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dis(0, agents.size() - 1);

    for(int i = 0; ; i++){
        message("Starting match " + std::to_string(i + 1) + " of " + std::to_string(matches) + "!", true);

        int agent1Index = focusAgent.value_or(dis(g));
        int agent2Index = dis(g);
        while(agent1Index == agent2Index){
            agent2Index = dis(g);
        }
        Agent *agent1 = agents[agent1Index].get();
        Agent *agent2 = agents[agent2Index].get();

        simulateMatch(agent1, agent2);
    }
}

void Tournament::roundRobin() const{
    for(int i = 0; i < agents.size(); i++){
        for(int j = i + 1; j < agents.size(); j++){
            message("Starting match " + std::to_string(i + 1) + " of " + std::to_string(agents.size() * (agents.size() - 1) / 2) + "!", true);
            simulateMatch(agents[i].get(), agents[j].get());
        }
    }
}


