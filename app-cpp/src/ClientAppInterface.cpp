#include "ClientAppInterface.hpp"

#include <iostream>
#include <string>

ClientAppInterface::ClientAppInterface(std::shared_ptr<ClientProcessingEngine> engine) : engine(engine) {
    // TO-DO implement constructor
}

void ClientAppInterface::readCommands() {
    // TO-DO implement the read commands method
    std::string command;
    
    while (true) {
        std::cout << "> ";
        
        // read from command line
        std::cin >> command;

        // if the command is quit, terminate the program       
        if (command == "quit") {
            // close the connection with the server
            engine->disconnect();
            break;
        }

        // if the command begins with connect, connect to the given server
        if (command.size() >= 7 && command.substr(0, 7) == "connect") {
            // TO-DO parse command cand call connect on the processing engine
            continue;
        }
        
        // if the command begins with index, index the files from the specified directory
        if (command.size() >= 5 && command.substr(0, 5) == "index") {
            // TO-DO parse command and call indexFolder on the processing engine
            // TO-DO print the execution time and the total number of bytes read
            continue;
        }

        // if the command begins with search, search for files that matches the query
        if (command.size() >= 6 && command.substr(0, 6) == "search") {
            // TO-DO parse command and call search on the processing engine
            // TO-DO print the execution time and the top 10 search results
            continue;
        }

        std::cout << "unrecognized command!" << std::endl;
    }
}