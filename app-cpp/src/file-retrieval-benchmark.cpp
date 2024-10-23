#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <numeric>
#include <fstream>
#include <sys/stat.h>
#include "ClientProcessingEngine.hpp"

void runWorker(ClientProcessingEngine &client, const std::string &datasetPath, long &totalBytesIndexed)
{

    auto result = client.indexFolder(datasetPath);

    totalBytesIndexed = result.totalBytesRead;
}

void performSearch(ClientProcessingEngine &client, const std::string &query)
{
    std::cout << "\nSearching " << query << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Split the query if it contains "AND" logic
    std::vector<std::string> searchTerms;
    if (query.find("AND") != std::string::npos) {
        size_t pos = 0;
        std::string token;
        std::string delimiter = " AND ";
        std::string modifiedQuery = query;
        
        while ((pos = modifiedQuery.find(delimiter)) != std::string::npos) {
            token = modifiedQuery.substr(0, pos);
            searchTerms.push_back(token);
            modifiedQuery.erase(0, pos + delimiter.length());
        }
        searchTerms.push_back(modifiedQuery);  
    } else {
        searchTerms.push_back(query);  
    }

    // Perform the search
    auto searchResult = client.search(searchTerms);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> searchDuration = end - start;

    std::cout << "Search completed in " << searchDuration.count() << " seconds" << std::endl;

    // Display top 10 search results
    auto &results = searchResult.documentFrequencies;
    size_t resultCount = std::min(results.size(), static_cast<size_t>(10));
    std::cout << "Search results (top " << resultCount << " out of " << results.size() << "):" << std::endl;

    for (size_t i = 0; i < resultCount; ++i) {
        const auto &doc = results[i];
        std::cout << "* " << doc.origin << ": " << doc.documentPath << ":" << doc.wordFrequency << std::endl;
    }
}

int main(int argc, char **argv)
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <num_clients> <client1_dataset> [<client2_dataset> ...]" << std::endl;
        return 1;
    }


    std::string serverIP = argv[1];
    std::string serverPort = argv[2];
    int numberOfClients = std::stoi(argv[3]);


    std::vector<std::string> clientsDatasetPath;
    for (int i = 4; i < argc; ++i) {
        clientsDatasetPath.push_back(argv[i]);
    }

    if (clientsDatasetPath.size() != numberOfClients) {
        std::cerr << "Error: Number of client datasets does not match the number of clients." << std::endl;
        return 1;
    }


    auto startTime = std::chrono::high_resolution_clock::now();


    std::vector<ClientProcessingEngine> clients(numberOfClients);

    for (int i = 0; i < numberOfClients; ++i) {
        if (!clients[i].connectToServer(serverIP, serverPort)) {
            std::cerr << "Error: Failed to connect client " << i + 1 << " to the server." << std::endl;
            return 1;
        }
    }


    std::vector<long> totalBytesIndexed(numberOfClients, 0);

    std::vector<std::thread> workers;
    for (int i = 0; i < numberOfClients; ++i) {
        workers.emplace_back(runWorker, std::ref(clients[i]), std::ref(clientsDatasetPath[i]), std::ref(totalBytesIndexed[i]));
    }

    for (auto &worker : workers) {
        worker.join();
    }

    auto stopTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> indexingDuration = stopTime - startTime;

    long totalBytes = std::accumulate(totalBytesIndexed.begin(), totalBytesIndexed.end(), 0L);
    double totalTime = indexingDuration.count();

    std::cout << "\nCompleted indexing " << totalBytes << " bytes of data\n";
    std::cout << "Completed indexing in " << totalTime << " seconds";


    std::vector<std::string> searchQueries = {"at", "Worms", "distortion AND adaptation"};

    for (const auto &query : searchQueries) {
        performSearch(clients[0], query);
    }

    return 0;
}
