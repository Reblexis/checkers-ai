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

int main()
{
#ifdef CLI
    // Build an executable agent

    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent = HyperparametersAgent(std::move(hyperparameters), "first_agent");
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef TOURNAMENT

    // Launch a tournament

    std::cout<<"Hello! Welcome to the tournament system!\nHere you can launch your own checkers tournament not only for the artificially intelligent agents.\n"
               "Please specify the path to your tournament config file (default: default_tournament.json): ";


    std::filesystem::path tournamentConfigFile;
    std::string tournamentConfigFileInput;
    std::getline(std::cin, tournamentConfigFileInput);
    tournamentConfigFile = tournamentConfigFileInput;
    if(tournamentConfigFileInput.empty()){
        tournamentConfigFile = DATA_PATH / "default_tournament.json";
    }
    Tournament tournament = Tournament::createFromFile(tournamentConfigFile);

#endif
}
