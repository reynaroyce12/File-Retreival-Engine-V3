#ifndef SERVER_SIDE_ENGINE_H
#define SERVER_SIDE_ENGINE_H

#include <memory>
#include <vector>
#include <string>

#include "IndexStore.hpp"

class ServerProcessingEngine {
    std::shared_ptr<IndexStore> store;
    // TO-DO keep track of the Dispatcher thread
    // TO-DO keep track of the Index Worker threads
    // TO-DO keep track of the clients information

    public:
        // constructor
        ServerProcessingEngine(std::shared_ptr<IndexStore> store);

        // default virtual destructor
        virtual ~ServerProcessingEngine() = default;

        void initialize(int serverPort);


        void runDispatcher();

        void runWorker();
        
        void shutdown();

        std::vector<std::string> getConnectedClients();
};

#endif