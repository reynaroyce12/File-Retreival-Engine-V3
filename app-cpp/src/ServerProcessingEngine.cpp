#include "ServerProcessingEngine.hpp"
#include "serverMessages.pb.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unordered_map>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstddef> 
#include <algorithm> 


ServerProcessingEngine::ServerProcessingEngine(std::shared_ptr<IndexStore> store) : store(store), running(true) {}

void ServerProcessingEngine::initialize(int serverPort) {
    dispatcherThread = std::thread(&ServerProcessingEngine::runDispatcher, this, serverPort);
}

// std::string ServerProcessingEngine::addClient(const std::string &clientIP, int clientPort) {

//     clientName = "client_" + std::to_string(connectedClients.size() + 1);
//     ClientInfo newClient = {clientName, clientIP, clientPort};

//     connectedClients.push_back(newClient);
//     return clientName;
// }

// void ServerProcessingEngine::spawnWorker(int clientSocket) {

//     sockaddr_in clientIpAddress;
//     socklen_t addressLength = sizeof(clientIpAddress);

//     if (getpeername(clientSocket, (sockaddr *)&clientIpAddress, &addressLength) == 0) {
//         std::string clientIP = inet_ntoa(clientIpAddress.sin_addr);
//         clientPort = ntohs(clientIpAddress.sin_port);

//         clientName = addClient(clientIP, clientPort);
//     } else {
//         std::cerr << "Failed to retrieve client information." << std::endl;
//         close(clientSocket);
//         return;
//     }

//     std::thread workerThread(&ServerProcessingEngine::runWorker, this, clientSocket);
//     workerThreads.push_back(std::move(workerThread));
// }

// void ServerProcessingEngine::runDispatcher(int serverPort)
// {

//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket < 0)
//     {
//         std::cout << "Error opening socket connection" << std::endl;
//         return;
//     }

//     sockaddr_in serverAddress;
//     memset(&serverAddress, 0, sizeof(serverAddress));
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = INADDR_ANY;
//     serverAddress.sin_port = htons(serverPort);

//     if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
//     {
//         std::cout << "Error binding the socket" << std::endl;
//         close(serverSocket);
//         return;
//     }

//     listen(serverSocket, 5);

//     std::cout << "Server is running successfully at port " << serverPort << std::endl;

//     while (true)
//     {

//             sockaddr_in clientAddress;
//             socklen_t clientLength = sizeof(clientAddress);
//             int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
//                     if (!termination)
//         {
//             if (clientSocket < 0)
//             {
//                 std::cout << "Error accepting connection." << std::endl;
//                 continue;
//             }

//             std::cout << "New client is connected!" << std::endl;

//             spawnWorker(clientSocket);
//         }
//         else
//         {
//             std::cout << "termination flag true" << std::endl;
//             close(serverSocket);
//             break;
//         }
//     }
//     // close(serverSocket);
// }

// Function to add client
std::string ServerProcessingEngine::addClient(const std::string &clientIP, int clientPort) {
    std::string clientName = "client_" + std::to_string(connectedClients.size() + 1);
    ClientInfo newClient = {clientName, clientIP, clientPort};

    connectedClients.push_back(newClient);
    return clientName;
}

// Function to spawn worker
void ServerProcessingEngine::spawnWorker(int clientSocket) {
    sockaddr_in clientIpAddress;
    socklen_t addressLength = sizeof(clientIpAddress);

    if (getpeername(clientSocket, (sockaddr *)&clientIpAddress, &addressLength) == 0) {
        std::string clientIP = inet_ntoa(clientIpAddress.sin_addr);
        int clientPort = ntohs(clientIpAddress.sin_port);

        std::string clientName = addClient(clientIP, clientPort);  // Use a local variable
        std::thread workerThread(&ServerProcessingEngine::runWorker, this, clientSocket, clientName);
        workerThreads.push_back(std::move(workerThread));
    } else {
        std::cerr << "Failed to retrieve client information." << std::endl;
        close(clientSocket);
        return;
    }
}


void ServerProcessingEngine::runDispatcher(int serverPort) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    // Set the socket to non-blocking mode
    fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL) | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return;
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port " << serverPort << std::endl;

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        // Timeout to avoid blocking
        struct timeval timeout;
        timeout.tv_sec = 1; // 1 second timeout
        timeout.tv_usec = 0;

        int activity = select(serverSocket + 1, &readfds, nullptr, nullptr, &timeout);
        if (activity < 0) {
            std::cerr << "Select error" << std::endl;
            continue;
        }

        if (activity == 0) {
            // Timeout occurred, check running flag
            continue;
        }

        // Accept new client if there's an activity
        if (FD_ISSET(serverSocket, &readfds)) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

            if (clientSocket < 0) {
                std::cerr << "Error accepting client connection" << std::endl;
                continue; // Continue to the next iteration to accept new connections
            }

            spawnWorker(clientSocket);
        }
    }

    close(serverSocket);
}

// std::vector<DocPathFreqPair> ServerProcessingEngine::runWorker(int clientSocket)
// {
//     const int bufferSize = 150 * 1024; // 150 KB
//     char buffer[bufferSize];

//     while (true)
//     {
//         int32_t messageLength = 0;
//         ssize_t bytesRead = read(clientSocket, &messageLength, sizeof(messageLength));
//         if (bytesRead != sizeof(messageLength))
//         {
//             std::cerr << "Failed to read message length." << std::endl;
//             close(clientSocket);
//             return {};
//         }

//         // Convert the message length from network byte order to host byte order
//         messageLength = ntohl(messageLength);

//         if (messageLength > sizeof(buffer))
//         {
//             std::cerr << "Message too large to fit in buffer." << std::endl;
//             close(clientSocket);
//             return {};
//         }

//         bytesRead = read(clientSocket, buffer, messageLength);
//         if (bytesRead != messageLength)
//         {
//             std::cerr << "Failed to read full message." << std::endl;
//             close(clientSocket);
//             return {};
//         }

//         // Convert buffer to a string for easier manipulation
//         std::string receivedMessage(buffer, bytesRead);

//         // Check the prefix to identify the request type
//         if (receivedMessage.starts_with("INDEX:"))
//         {

//             // Remove the prefix to get the actual message
//             std::string actualMessage = receivedMessage.substr(strlen("INDEX:"));
//             IndexRequest indexRequest;

//             // Parse the IndexRequest
//             if (indexRequest.ParseFromString(actualMessage))
//             {
//                 // Handle IndexRequest
//                 long documentNumber = store->putDocument(indexRequest.document_path(), clientName);
//                 std::unordered_map<std::string, long> wordFrequenciesMap;

//                 for (const auto &pair : indexRequest.word_frequencies())
//                 {
//                     wordFrequenciesMap[pair.first] = static_cast<long>(pair.second);
//                 }

//                 store->updateIndex(documentNumber, wordFrequenciesMap);

//                 IndexReply reply;
//                 reply.set_status("Indexing complete");
//                 std::string replyData;
//                 reply.SerializeToString(&replyData);

//                 // Include message size before sending the reply
//                 uint32_t sizeToSend = htonl(static_cast<uint32_t>(replyData.size()));
//                 char replyBuffer[sizeof(sizeToSend) + replyData.size()];
//                 memcpy(replyBuffer, &sizeToSend, sizeof(sizeToSend));
//                 memcpy(replyBuffer + sizeof(sizeToSend), replyData.data(), replyData.size());

//                 send(clientSocket, replyBuffer, sizeof(sizeToSend) + replyData.size(), 0);
//                 continue;
//             }
//             else
//             {
//                 std::cerr << "Failed to parse IndexRequest." << std::endl;
//             }
//         }
//         else if (receivedMessage.starts_with("SEARCH:"))
//         {
//             std::string actualMessage = receivedMessage.substr(strlen("SEARCH:"));
//             SearchRequest searchRequest;

//             // Parse the SearchRequest
//             if (searchRequest.ParseFromString(actualMessage))
//             {

//                 std::unordered_map<long, long> combinedResults; // Document number and combined frequency

//                 for (const auto &term : searchRequest.terms())
//                 {
//                     if (term.empty())
//                         continue;

//                     auto termResults = store->lookupIndex(term); // Assuming lookupIndex returns pairs of {document_number, frequency}

//                     if (termResults.empty())
//                     {
//                         // If no results for the current term, just skip it
//                         continue;
//                     }

//                     if (combinedResults.empty())
//                     {
//                         // If this is the first term, add all its results to combinedResults
//                         for (const auto &result : termResults)
//                         {
//                             combinedResults[result.documentNumber] = result.wordFrequency;
//                         }
//                     }
//                     else
//                     {
//                         // If not the first term, update frequencies for documents already in combinedResults
//                         std::unordered_map<long, long> currentResults;
//                         for (const auto &result : termResults)
//                         {
//                             if (combinedResults.count(result.documentNumber))
//                             {
//                                 currentResults[result.documentNumber] = combinedResults[result.documentNumber] + result.wordFrequency;
//                             }
//                         }
//                         combinedResults = std::move(currentResults); // Retain only those found in previous terms
//                     }
//                 }

//                 // Prepare the search reply
//                 SearchReply searchReply;
//                 searchReply.set_execution_time(0.0); // Set execution time (you can measure this)

//                 if (combinedResults.empty())
//                 {
//                     std::cout << "No documents match all search terms." << std::endl;
//                     // Send an empty SearchReply
//                     searchReply.set_total_results(0);
//                 }
//                 else
//                 {
//                     // Prepare sorted results
//                     std::vector<std::pair<long, long>> sortedResults(combinedResults.begin(), combinedResults.end());
//                     std::sort(sortedResults.begin(), sortedResults.end(),
//                               [](const auto &a, const auto &b)
//                               { return a.second > b.second; });
//                     if (sortedResults.size() > 10)
//                     {
//                         sortedResults.resize(10); // Keep only top 10
//                     }

//                     for (const auto &result : sortedResults)
//                     {
//                         long docNumber = result.first;
//                         long frequency = result.second;

//                         SearchReply::Document *doc = searchReply.add_documents();
//                         DocumentInfo docInfo = store->getDocument(docNumber);
//                         doc->set_document_path(docInfo.docPath); 
//                         doc->set_frequency(frequency);
//                         doc->set_client_id(docInfo.origin);
//                     }

//                     searchReply.set_total_results(sortedResults.size());
//                 }

//                 // Serialize reply message
//                 std::string searchReplyData;
//                 searchReply.SerializeToString(&searchReplyData);

//                 // Include message size before sending the reply
//                 uint32_t sizeToSend = htonl(static_cast<uint32_t>(searchReplyData.size()));
//                 char replyBuffer[sizeof(sizeToSend) + searchReplyData.size()];
//                 memcpy(replyBuffer, &sizeToSend, sizeof(sizeToSend));
//                 memcpy(replyBuffer + sizeof(sizeToSend), searchReplyData.data(), searchReplyData.size());

//                 send(clientSocket, replyBuffer, sizeof(sizeToSend) + searchReplyData.size(), 0);
//                 continue;
//             }
//             else
//             {
//                 std::cerr << "Failed to parse SearchRequest." << std::endl;
//             }
//         } else if(receivedMessage.contains("QUIT")) {
//             std::cout << "Client sent QUIT message." << std::endl;


//             auto it = std::find_if(connectedClients.begin(), connectedClients.end(),
//                                    [this](const ClientInfo &client)
//                                    {
//                                        return client.clientPort == clientPort; 
//                                    });

//             if (it != connectedClients.end())
//             {

//                 std::cout << it->clientName << " with IP " << it->clientIP
//                           << " and port " << it->clientPort << " disconnected." << std::endl;

//                 // Remove the client from connectedClients
//                 connectedClients.erase(it);
//             }

//             close(clientSocket);
//             return {};
//         }
//         else
//         {
//             std::cerr << "Unknown request type." << std::endl;
//         }
//     }
// }

//!jestin comment
std::vector<DocPathFreqPair> ServerProcessingEngine::runWorker(int clientSocket, const std::string &clientName)
{
    while (true)
    {
        uint32_t dataSize;
        recv(clientSocket, &dataSize, sizeof(dataSize), 0);
        dataSize = ntohl(dataSize);

        std::vector<char> buffer(dataSize);
        size_t totalBytesReceived = 0;

        size_t bytesReceived;

        while (totalBytesReceived < dataSize) {
            bytesReceived = recv(clientSocket, buffer.data() + totalBytesReceived, dataSize - totalBytesReceived, 0);
            totalBytesReceived += bytesReceived;
        }

        std::string receivedMessage(buffer.begin(), buffer.end());
        usleep(50000);
// std::cout << "message: " << receivedMessage << std::endl;
        // Check the prefix to identify the request type
        if (receivedMessage.starts_with("INDEX:"))
        {
            std::cout << "here" << std::endl;
            // Remove the prefix to get the actual message
            std::string actualMessage = receivedMessage.substr(strlen("INDEX:"));
            IndexRequest indexRequest;

            // Parse the IndexRequest
            if (indexRequest.ParseFromString(actualMessage))
            {
                // std::cout << "here_1" << std::endl;
                // Handle IndexRequest
                long documentNumber = store->putDocument(indexRequest.document_path(), clientName);
                std::unordered_map<std::string, long> wordFrequenciesMap;

                for (const auto &pair : indexRequest.word_frequencies())
                {
                    wordFrequenciesMap[pair.first] = static_cast<long>(pair.second);
                }

                store->updateIndex(documentNumber, wordFrequenciesMap);
                // std::cout << "here_2" << std::endl;
                std::string reply = "Index updated successfully";
                send(clientSocket, reply.c_str(), reply.size(), 0);

            } else {
                std::cerr << "Failed to parse IndexRequest." << std::endl;
            }

        } else if (receivedMessage.starts_with("SEARCH:")) {
            std::string actualMessage = receivedMessage.substr(strlen("SEARCH:"));
            SearchRequest searchRequest;

            // Parse the SearchRequest
            if (searchRequest.ParseFromString(actualMessage))
            {

                std::unordered_map<long, long> combinedResults; // Document number and combined frequency

                for (const auto &term : searchRequest.terms())
                {
                    if (term.empty())
                        continue;

                    auto termResults = store->lookupIndex(term); // Assuming lookupIndex returns pairs of {document_number, frequency}

                    if (termResults.empty())
                    {
                        // If no results for the current term, just skip it
                        continue;
                    }

                    if (combinedResults.empty())
                    {
                        // If this is the first term, add all its results to combinedResults
                        for (const auto &result : termResults)
                        {
                            combinedResults[result.documentNumber] = result.wordFrequency;
                        }
                    }
                    else
                    {
                        // If not the first term, update frequencies for documents already in combinedResults
                        std::unordered_map<long, long> currentResults;
                        for (const auto &result : termResults)
                        {
                            if (combinedResults.count(result.documentNumber))
                            {
                                currentResults[result.documentNumber] = combinedResults[result.documentNumber] + result.wordFrequency;
                            }
                        }
                        combinedResults = std::move(currentResults); // Retain only those found in previous terms
                    }
                }

                // Prepare the search reply
                SearchReply searchReply;
                searchReply.set_execution_time(0.0); // Set execution time (you can measure this)

                if (combinedResults.empty())
                {
                    std::cout << "No documents match all search terms." << std::endl;
                    // Send an empty SearchReply
                    searchReply.set_total_results(0);
                    
                }
                else
                {
                    // Prepare sorted results
                    std::vector<std::pair<long, long>> sortedResults(combinedResults.begin(), combinedResults.end());
                    std::sort(sortedResults.begin(), sortedResults.end(),
                              [](const auto &a, const auto &b)
                              { return a.second > b.second; });
                    if (sortedResults.size() > 10)
                    {
                        sortedResults.resize(10); // Keep only top 10
                    }

                    for (const auto &result : sortedResults)
                    {
                        long docNumber = result.first;
                        long frequency = result.second;

                        SearchReply::Document *doc = searchReply.add_documents();
                        DocumentInfo docInfo = store->getDocument(docNumber);
                        doc->set_document_path(docInfo.docPath); 
                        doc->set_frequency(frequency);
                        doc->set_client_id(docInfo.origin);
                    }

                    searchReply.set_total_results(sortedResults.size());
                }

                // Serialize reply message
                std::string searchReplyData;
                searchReply.SerializeToString(&searchReplyData);

                std::cout<< "SearchReplyData" << searchReplyData <<std::endl;

                // Include message size before sending the reply
                uint32_t sizeToSend = htonl(static_cast<uint32_t>(searchReplyData.size()));
                char replyBuffer[sizeof(sizeToSend) + searchReplyData.size()];
                memcpy(replyBuffer, &sizeToSend, sizeof(sizeToSend));
                memcpy(replyBuffer + sizeof(sizeToSend), searchReplyData.data(), searchReplyData.size());

                send(clientSocket, replyBuffer, sizeof(sizeToSend) + searchReplyData.size(), 0);
                continue;
            }
            else
            {
                std::cerr << "Failed to parse SearchRequest." << std::endl;
            }
        } else if(receivedMessage.contains("QUIT")) {
            std::cout << "Client sent QUIT message." << std::endl;


            auto it = std::find_if(connectedClients.begin(), connectedClients.end(),
                                   [this](const ClientInfo &client)
                                   {
                                       return client.clientPort == clientPort; 
                                   });

            if (it != connectedClients.end())
            {

                std::cout << it->clientName << " with IP " << it->clientIP
                          << " and port " << it->clientPort << " disconnected." << std::endl;

                // Remove the client from connectedClients
                connectedClients.erase(it);
            }

            close(clientSocket);
            return {};
        }
        else
        {
            std::cerr << "Unknown request type." << std::endl;
        }
    }
}


void ServerProcessingEngine::shutdown() {
    running = false; 

    for (auto& workerThread : workerThreads) {
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    if (dispatcherThread.joinable()) {
        dispatcherThread.join();
    }

    std::cout << "Server has shut down gracefully." << std::endl;
}


std::vector<std::string> ServerProcessingEngine::getConnectedClients()
{
    std::vector<std::string> clientList;

    for (const auto &client : connectedClients)
    {
        std::string clientInfo = client.clientName + ": " + client.clientIP + " " + std::to_string(client.clientPort);
        clientList.push_back(clientInfo);
    }
    return clientList;
}