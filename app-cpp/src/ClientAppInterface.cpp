#include "ClientAppInterface.hpp"

#include <iostream>
#include <string>
#include <sstream>

ClientAppInterface::ClientAppInterface(std::shared_ptr<ClientProcessingEngine> engine) : engine(engine) {
    // TO-DO implement constructor
}

void ClientAppInterface::readCommands() {

    std::string command;

    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";

    while (true) {
        std::cout << "> <connect | index | search | quit>  ";
        

        std::getline(std::cin, command);


        if (command == "quit") {
            engine->disconnect();
            break;
        }

        if (command.size() >= 7 && command.substr(0, 7) == "connect") {

            std::istringstream iss(command);
            std::string action, serverIP;
            std::string serverPort;

            iss >> action >> serverIP >> serverPort;

            if(serverIP.empty() || serverPort.empty()) {
                std::cout << "Invalid connection command" << std::endl;
                continue;
            }

            if(engine->connectToServer(serverIP, serverPort)) {
                std::cout << "Connection Successfull!" << std::endl;
            } else {
                std::cout << "Failed to connect to the server!" << std::endl;
            }

            continue;
        }
        

        if (command.size() >= 5 && command.substr(0, 5) == "index") {
            std::string folderPath = command.substr(6);

            if(folderPath.empty()) {
                std::cout << "Please enter a valid folder path." << std::endl;
                continue;
            }

            auto result = engine->indexFolder(folderPath);
            std::cout << "Completed indexing " << result.totalBytesRead << " bytes of data" << std::endl;
            std::cout << "Completed indexing in " << result.executionTime << " seconds" << std::endl;

            continue;
        }

        // if the command begins with search, search for files that matches the query
        if (command.size() >= 6 && command.substr(0, 6) == "search") {

            std::string searchQuery = command.substr(7);

            if (searchQuery.empty()) {
                std::cout << "Please enter the search terms." << std::endl;
                continue;
            }

            std::vector<std::string> terms;
            std::istringstream stream(searchQuery);
            std::string term;

            while (stream >> term) {
                terms.push_back(term);
            }

            SearchResult result = engine->search(terms);

            std::cout << "\nSearch completed in " << result.executionTime << " seconds." << std::endl;

            if (result.documentFrequencies.empty()) {
                std::cout << YELLOW << "No results found" << RESET << std::endl;
            } else {
                std::cout << "Search Results: " << "( Top 10 out of " << result.documentFrequencies.size() << "): \n"<< std::endl;
                for (const auto &docFrequency : result.documentFrequencies) {
                    std::cout << GREEN << docFrequency.origin << ": " << docFrequency.documentPath << " (Frequency: " << docFrequency.wordFrequency << ")" << RESET << std::endl;
                }
            }

            continue;
        }

        std::cout << "unrecognized command!" << std::endl;
    }
}