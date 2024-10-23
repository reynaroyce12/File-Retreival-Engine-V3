#include "ServerAppInterface.hpp"

#include <iostream>
#include <vector>
#include <string>

ServerAppInterface::ServerAppInterface(std::shared_ptr<ServerProcessingEngine> engine) : engine(engine) { }

void ServerAppInterface::readCommands() {
    // TO-DO implement the read commands method
    std::string command;
    
    while (true) {
        std::cout << "> <list | quit>  ";
        
        // read from command line
        std::getline(std::cin, command);


        // if the command is quit, terminate the program       
        if (command == "quit") {
            engine->shutdown();
            break;
        }

        // if the command begins with list, list all the connected clients
        else if (command.size() >= 4 && command.substr(0, 4) == "list") {
            // TO-DO call the getConnectedClients method from the server to retrieve the clients information
            // TO-DO print the clients information
            std::vector<std::string> clientsInformation = engine->getConnectedClients();
            if (clientsInformation.empty()) {
                std::cout << "No clients connected." << std::endl;
            } else {
                for (const std::string &clientInfo : clientsInformation) {
                    std::cout << clientInfo << std::endl;
                }
            }
        } else {
        std::cout << "unrecognized command!" << std::endl;

        }

    }
}