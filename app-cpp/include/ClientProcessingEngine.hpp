#ifndef CLIENT_SIDE_ENGINE_H
#define CLIENT_SIDE_ENGINE_H

#include "serverMessages.pb.h"

#include <memory>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>

#include "IndexStore.hpp"

struct IndexResult {
    double executionTime;
    long totalBytesRead;
};

struct DocPathFreqPair {
    std::string documentPath;
    long wordFrequency;
    std::string origin;
};

struct SearchResult {
    double executionTime;
    std::vector<DocPathFreqPair> documentFrequencies;
};

struct IndexRequestStruct {
    std::string clientID;
    std::string documentPath;
    std::unordered_map<std::string, int> wordFrequencies;
};

struct ClientInfo {
    std::string clientID;
    int socket;
};

class ClientProcessingEngine {
    // TO-DO keep track of the connection (socket) ✅
    private:
        int clientSocket;
        struct sockaddr_in serverAddress;
        std::unordered_map<int, ClientInfo> clientMap;
        std::mutex clientMutex;

    public:
        // constructor
        ClientProcessingEngine();

        // default virtual destructor
        virtual ~ClientProcessingEngine() = default;

        IndexResult indexFolder(std::string folderPath);
        
        SearchResult search(std::vector<std::string> terms);
        
        bool connectToServer(std::string serverIP, std::string serverPort);
        
        void disconnect();

    // Utility functions for the indexFolder and search method
    private:
        std::unordered_map<std::string, int> extractWords(const std::string& fileContent);
        std::vector<DocPathFreqPair> searchAndSort(std::vector<std::string> terms);

        std::string generateClientID();
        void removeClientFromMap(int clientSocket);
        bool sendIndexRequest(const IndexRequest& request);
        SearchResult sendMessageAndReceiveResponse(const std::string& message);
        
};

#endif

