#include "ServerProcessingEngine.hpp"

ServerProcessingEngine::ServerProcessingEngine(std::shared_ptr<IndexStore> store) : store(store) { }

void ServerProcessingEngine::initialize(int serverPort) {
    // TO-DO create and start the Dispatcher thread
}

void ServerProcessingEngine::runDispatcher() {
    // TO-DO create a TCP/IP socket and listen for new connections
    // TO-DO When new connection comes through create a new worker thread for the new connection
    // TO-DO Use the engine spawnWorker method to create a new worker thread
}

void ServerProcessingEngine::runWorker() {
    // TO-DO receive a message from the client
    // TO-DO if the message is an INDEX REQUEST, then
    //       extract the document path, client ID and word frequencies from the message(s)
    //       get the document number associated with the document path (call putDocument)
    //       update the index store with the word frequencies and the document number
    //       return an acknowledgement INDEX REPLY message
    // TO-DO if the message is a SEARCH REQUEST, then
    //       extract the terms from the message
    //       for each term get the pairs of documents and frequencies from the index store
    //       combine the returned documents and frequencies from all of the specified terms
    //       sort the document and frequency pairs and keep only the top 10
    //       for each document number get from the index store the document path
    //       return a SEARCH REPLY message containing the top 10 results
    // TO-DO if the message is a QUIT message, then finish running
}

void ServerProcessingEngine::shutdown() {
    // TO-DO signal the Dispatcher thread to shutdown
    // TO-DO join the Dispatcher and worker threads
}

std::vector<std::string> ServerProcessingEngine::getConnectedClients() {
    // TO-DO return the connected clients information
    return std::vector<std::string>();
    
}