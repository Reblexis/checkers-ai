#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "includes/constants.hpp"

#include "communication/includes/game.hpp"
#include "ai/includes/agent.hpp"
#include "app/includes/app.hpp"
#include "ai/includes/console_interface.hpp"
#include "meta/includes/tournament.hpp"

//#define CLI
#define TOURNAMENT
//#define TESTS

int main()
{
#ifdef CLI
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent = HyperparametersAgent(std::move(hyperparameters), "first_agent");
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef TOURNAMENT

    std::cout<<"Hello! Welcome to the tournament system!\nHere you can launch your own checkers tournament not only for the artificially intelligent agents.\n"
               "Please specify the path to your tournament config file (default: tournament.json): ";


    std::filesystem::path tournamentConfigFile;
    std::string tournamentConfigFileInput;
    std::getline(std::cin, tournamentConfigFileInput);
    tournamentConfigFile = tournamentConfigFileInput;
    if(tournamentConfigFileInput.empty()){
        tournamentConfigFile = DATA_PATH / "tournament.json";
    }
    Tournament tournament = Tournament::createFromFile(tournamentConfigFile);

#endif
#ifdef TESTS
    Tournament tournament("test", true, 60, 0);
    std::vector<Agent *> agents;
    agents.push_back(new HyperparametersAgent("/home/cihalvi/src/checkers/data/default_hyperparameters.json", "agent1"));
    agents.push_back(new ExecutableAgent("/home/cihalvi/src/checkers/data/agents/executable_agent/checkers", "agent2"));
    tournament.roundRobin(agents);
#endif
}
