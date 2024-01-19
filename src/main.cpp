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
//#define TOURNAMENT
#define TESTS

int main()
{
#ifdef CLI
    Hyperparameters hyperparameters = Hyperparameters(CURRENT_AGENT_CONFIG);
    HyperparametersAgent agent = HyperparametersAgent(std::move(hyperparameters), "first_agent");
    ConsoleInterface consoleInterface(&agent);
    consoleInterface.run();
#endif
#ifdef TOURNAMENT
    std::string tournamentID;
    int numberOfAgents;
    std::vector<Agent *> agents;

    std::cout<<"Hello! Welcome to the tournament system!\nHere you can create your own checkers tournament not only for the artificially intelligent agents.\n"
               "First, you have to specify a few things for your tournament.\n";
    std::cout<<"Please enter the tournament ID (no special characters): ";
    std::cin>>tournamentID;
    std::cout<<"Great! Now, specify the agents that will be taking part in the tournament!\n";
    std::cout<<"Number of agents: ";
    std::cin>>numberOfAgents;
    for(int i = 0; i < numberOfAgents; i++){
        char agentType;
        std::filesystem::path agentPath;

        std::cout<<std::format("Agent ({}/{}): \n", i+1, numberOfAgents);
        std::cout<<"Agent ID: ";
        std::string agentID;
        std::cin>>agentID;
        std::cout<<"Type of the agent (h/e/p), h = hyperparameters agent, e = executable agent, p = player: ";
        std::cin>>agentType;

        if(agentType == 'h' || agentType=='e'){
            std::cout<<"Full or local path to the agent file:";
            std::cin>>agentPath;
        }

        Agent* agent;
        if(agentType == 'h'){
            agent = new HyperparametersAgent(agentPath, agentID);
        }else if(agentType == 'e'){
            agent = new ExecutableAgent(agentPath, agentID);
        }else if(agentType == 'p'){
            agent = new Player(agentID);
        }
        agents.push_back(agent);
    }

    std::cout<<"Launching tournament!\n";
    // TODO: Add advanced settings support

    Tournament tournament(tournamentID, true, 60, 0);
    tournament.roundRobin(agents);
#endif
#ifdef TESTS
    Tournament tournament("test", false, 60, 0);
    std::vector<Agent *> agents;
    agents.push_back(new HyperparametersAgent("/home/cihalvi/src/checkers/data/default_hyperparameters.json", "agent1"));
    agents.push_back(new Player( "agent2"));
    tournament.roundRobin(agents);
#endif
}
