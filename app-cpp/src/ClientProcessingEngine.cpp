#include "ClientProcessingEngine.hpp"
#include <serverMessages.pb.h>

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <arpa/inet.h>



ClientProcessingEngine::ClientProcessingEngine() {
    clientSocket = -1;
}

std::string ClientProcessingEngine::generateClientID() {
    std::lock_guard<std::mutex> lock(clientMutex); 
    return std::to_string(clientMap.size() + 1);
}

std::unordered_map<std::string, int> ClientProcessingEngine::extractWords(const std::string& fileContent) {
    std::unordered_map<std::string, int> wordFrequency;
    std::string currentWord;

    for (char charecter : fileContent) {
        if (std::isalnum(charecter)) {
            currentWord += charecter;
        } else {
            if (currentWord.length() > 2) {
                wordFrequency[currentWord]++;
            }
            currentWord.clear();
        }
    }

    if(currentWord.length() > 2) {
        wordFrequency[currentWord]++;
    }
    return wordFrequency;
}


bool ClientProcessingEngine::sendIndexRequest(const IndexRequest& request) {
    std::string serializedData;

    // Serialize the IndexRequest
    request.SerializeToString(&serializedData);
    const std::string prefix = "INDEX:";
    std::string prefixedData = prefix + serializedData;

    // Send data size
    uint32_t dataSize = htonl(prefixedData.size());
    if (send(clientSocket, &dataSize, sizeof(dataSize), 0) <= 0) {
        std::cerr << "Error sending data size: " << strerror(errno) << std::endl;
        return false;
    }

    // Send actual data with handling for partial sends
    ssize_t bytesSent = 0;
    while (bytesSent < prefixedData.size()) {
        ssize_t result = send(clientSocket, prefixedData.c_str() + bytesSent, prefixedData.size() - bytesSent, 0);
        if (result < 0) {
            std::cerr << "Error sending IndexRequest: " << strerror(errno) << std::endl;
            return false;
        }
        bytesSent += result;
    }
    // std::cout << "bytesSent: " << bytesSent << std::endl;


    char buffer[10240] = {0}; 
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving reply data: " << strerror(errno) << std::endl;
        return false;
    }
    if (bytesReceived == 0) {
        std::cerr << "Server closed the connection prematurely." << std::endl;
        return false;
    }

    return true;
}

IndexResult ClientProcessingEngine::indexFolder(std::string folderPath) {
    IndexResult result = {0.0, 0};
    auto indexingStartTime = std::chrono::steady_clock::now();
    std::queue<std::string> fileQueue;
    std::mutex fileQueueMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads; 
    bool isDone = false;

    // Load files into the queue
    for (const auto& file : std::filesystem::recursive_directory_iterator(folderPath)) {
        if (file.is_regular_file()) {
            std::lock_guard<std::mutex> lock(fileQueueMutex);
            fileQueue.push(file.path().string());
        }
    }

    // Worker thread function
    auto worker = [&]() {
        while (true) {
            std::string filePath;

            {
                std::unique_lock<std::mutex> lock(fileQueueMutex);
                cv.wait(lock, [&]() { return !fileQueue.empty() || isDone; });

                if (fileQueue.empty()) {
                    if (isDone) return;
                    continue;
                }

                filePath = fileQueue.front();
                fileQueue.pop();
            }

            if (!filePath.empty()) {
                std::ifstream file(filePath);
                if (!file) {
                    std::cout << "Cannot open the file: " << filePath << std::endl;
                    continue;
                }

                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                auto wordFrequency = extractWords(content);

                IndexRequest request;
                request.set_client_id(generateClientID());
                request.set_document_path(filePath);
                for (const auto& pair : wordFrequency) {
                    (*request.mutable_word_frequencies())[pair.first] = pair.second; // Populate the word frequencies map
                }

                if (sendIndexRequest(request)) {
                    result.totalBytesRead += content.size(); // Accumulate total bytes read
                } else {
                    std::cerr << "Failed to send index request" << std::endl;
                }
            }
        }
    };

    for (int i = 0; i < 6; i++) {
        threads.emplace_back(worker);
    }

    // Notify threads when work is done
    {
        std::lock_guard<std::mutex> lock(fileQueueMutex);
        isDone = true;
    }
    cv.notify_all();

    // Joining threads
    for (auto& thread : threads) {
        thread.join();
    }

    auto indexingStopTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::seconds>(indexingStopTime - indexingStartTime).count();

    usleep(50000);
    return result;
}


bool ClientProcessingEngine::connectToServer(std::string serverIP, std::string serverPort) {

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cout << "Error opening socket" << std::endl;
        return false;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(std::stoi(serverPort));
    if(inet_pton(AF_INET, serverIP.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << "Invalid IP Address" << std::endl;
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection to server failed" << std::endl;
        close(clientSocket);
        return false;
    }

    std::string newClientId = generateClientID();
    clientMap[clientSocket] = {newClientId, clientSocket}; 
    return true;
}

void ClientProcessingEngine::removeClientFromMap(int clientSocket) {
    auto itr = clientMap.find(clientSocket);
    if (itr != clientMap.end()) {
        clientMap.erase(itr);
    }
}

void ClientProcessingEngine::disconnect() {

    std::string quitMessage = "QUIT";
    int32_t messageLength = htonl(quitMessage.size());              
    send(clientSocket, &messageLength, sizeof(messageLength), 0);   
    send(clientSocket, quitMessage.c_str(), quitMessage.size(), 0); // Send the message

    if (clientSocket >= 0)
    {
        close(clientSocket);
        removeClientFromMap(clientSocket);
        clientSocket = -1;
        std::cout << "Disconnected from server." << std::endl;
    }
}


SearchResult ClientProcessingEngine::sendMessageAndReceiveResponse(const std::string& message) {
    const std::string prefix = "SEARCH:";
    
    std::string prefixedMessage = prefix + message;

    // Create a buffer for sending the message size and data
    uint32_t messageSize = htonl(static_cast<uint32_t>(prefixedMessage.size())); // Convert size to network byte order
    char buffer[sizeof(messageSize) + prefixedMessage.size()];

    memcpy(buffer, &messageSize, sizeof(messageSize));
    memcpy(buffer + sizeof(messageSize), prefixedMessage.data(), prefixedMessage.size());

    // Send the buffer to the server
    ssize_t bytesSent = send(clientSocket, buffer, sizeof(messageSize) + prefixedMessage.size(), 0);
    if (bytesSent < 0) {
        std::cerr << "Error sending message: " << strerror(errno) << std::endl;
        return {}; 
    }

    // Receive the response size first
    uint32_t responseSize;
    ssize_t bytesReceived = recv(clientSocket, &responseSize, sizeof(responseSize), 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving response size: " << strerror(errno) << std::endl;
        return {};
    }



    responseSize = ntohl(responseSize); // Converting response size from network byte order


    // If no docs found return empty object
    if (!responseSize) {
        return {}; 
    }

    std::string response(responseSize, '\0');
    bytesReceived = recv(clientSocket, &response[0], responseSize, 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving response data: " << strerror(errno) << std::endl;
        return {}; 
    }


    // Deserialize the response
    SearchReply searchReply;
    if (!searchReply.ParseFromString(response)) {
        std::cerr << "Received response size: " << response.size() << std::endl;
        std::cerr << "Response content: " << response << std::endl; // Inspect the raw response
        std::cerr << "Failed to parse SearchReply." << std::endl;
        return {};
    }

    SearchResult result;
    result.executionTime = searchReply.execution_time();


    for (const auto& doc : searchReply.documents()) {
        DocPathFreqPair docFrequency;
        docFrequency.documentPath = doc.document_path();
        docFrequency.wordFrequency = doc.frequency();
        docFrequency.origin = doc.client_id();
        result.documentFrequencies.push_back(docFrequency);
    }

    return result; 
}


SearchResult ClientProcessingEngine::search(std::vector<std::string> terms) {
    SearchResult result = {0.0, {}}; 


    auto searchStartTime = std::chrono::steady_clock::now();


    SearchRequest request;
    for (const auto& term : terms) {
        request.add_terms(term); 
    }


    std::string serializedRequest;
    if (!request.SerializeToString(&serializedRequest)) {
        std::cerr << "Failed to serialize SearchRequest." << std::endl;
        return result; 
    }


    result = sendMessageAndReceiveResponse(serializedRequest); 


    if (result.documentFrequencies.empty() && result.executionTime == 0.0) {
        return result; 
    }


    auto searchStopTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::seconds>(searchStopTime - searchStartTime).count();

    return result;
}
