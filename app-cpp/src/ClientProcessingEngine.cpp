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
    // Receive reply data with handling for partial receives
    char buffer[10240] = {0};  // Increased buffer size
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

    // Create worker threads
    for (int i = 0; i < 6; i++) {
        threads.emplace_back(worker);
    }

    // Notify threads that work is done
    {
        std::lock_guard<std::mutex> lock(fileQueueMutex);
        isDone = true;
    }
    cv.notify_all();

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    auto indexingStopTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::seconds>(indexingStopTime - indexingStartTime).count();
    // std::cout << "result :" << result.executionTime << " " << result.totalBytesRead << std::endl;
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
    } else {
        // std::cerr << "Client with socket " << clientSocket << " not found in the map." << std::endl;
    }
}

void ClientProcessingEngine::disconnect() {
    // TO-DO implement disconnect from server ✅
    // TO-DO send a QUIT message to the server ✅
    // close the TCP/IP socket ✅
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
        return {}; // Handle send failure
    }

    // Receive the response size first
    uint32_t responseSize;
    ssize_t bytesReceived = recv(clientSocket, &responseSize, sizeof(responseSize), 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving response size: " << strerror(errno) << std::endl;
        return {};
    }



    responseSize = ntohl(responseSize); // Convert response size from network byte order


    if (!responseSize) {
        return {}; 
    }

    std::string response(responseSize, '\0');
    bytesReceived = recv(clientSocket, &response[0], responseSize, 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving response data: " << strerror(errno) << std::endl;
        return {}; // Handle receive failure
    }


    // Deserialize the response
    SearchReply searchReply;
    if (!searchReply.ParseFromString(response)) {
        std::cerr << "Received response size: " << response.size() << std::endl;
        std::cerr << "Response content: " << response << std::endl; // Inspect the raw response
        std::cerr << "Failed to parse SearchReply." << std::endl;
        return {}; // Handle parsing failure
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
    SearchResult result = {0.0, {}}; // Initialize result with execution time and empty document frequencies

    // Get the start time for execution time calculation
    auto searchStartTime = std::chrono::steady_clock::now();

    // Prepare the SEARCH REQUEST message
    SearchRequest request;
    for (const auto& term : terms) {
        request.add_terms(term); // Add each term to the request
    }

    // Serialize the SEARCH REQUEST to a string
    std::string serializedRequest;
    if (!request.SerializeToString(&serializedRequest)) {
        std::cerr << "Failed to serialize SearchRequest." << std::endl;
        return result; // Return default result if serialization fails
    }

    // Send the message and receive the response
    result = sendMessageAndReceiveResponse(serializedRequest); // Store the result directly from the response

    // Check if the result indicates an error or empty response
    if (result.documentFrequencies.empty() && result.executionTime == 0.0) {
        // std::cerr << "No response received from the server or response parsing failed." << std::endl;
        return result; // Return default result if no response
    }

    // Get the stop time and calculate the execution time
    auto searchStopTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::seconds>(searchStopTime - searchStartTime).count();

    return result; // Return the populated SearchResult
}
