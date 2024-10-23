#ifndef SERVER_SIDE_ENGINE_H
#define SERVER_SIDE_ENGINE_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>

#include "IndexStore.hpp"

struct DocPathFreqPair {
    std::string documentPath;
    long wordFrequency;
};

struct ClientInfo {
    std::string clientName;
    std::string clientIP;
    int clientPort;
};

class ServerProcessingEngine {
    std::shared_ptr<IndexStore> store;
    // TO-DO keep track of the Dispatcher thread
    // TO-DO keep track of the Index Worker threads
    // TO-DO keep track of the clients information
    // struct clientInformation {
    //     std::string clientID;
    //     std::string clientIP;
    //     int port;
    // };
    std::thread dispatcherThread;
    std::vector<std::thread> workerThreads;
    // std::unordered_map<int, clientInformation> clients;
    std::atomic<bool> termination{false};
    int serverSocket;
    std::vector<ClientInfo> connectedClients;
    std::string clientName;
    std::mutex clientsMutex; 
    bool running = true;
    int clientPort;


    public:
        // constructor
        ServerProcessingEngine(std::shared_ptr<IndexStore> store);

        // default virtual destructor
        virtual ~ServerProcessingEngine() = default;

        void initialize(int serverPort);


        void runDispatcher(int serverPort);

        void spawnWorker(int clientSocket);

        std::vector<DocPathFreqPair> runWorker(int clientSocket, const std::string &clientName);
        
        void shutdown();

        std::vector<std::string> getConnectedClients();

        std::string addClient(const std::string& clientIP, int clientPort);
};

#endif