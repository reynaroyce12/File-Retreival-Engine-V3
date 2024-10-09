#include "ClientProcessingEngine.hpp"

ClientProcessingEngine::ClientProcessingEngine() {
    // TO-DO add information to keep track of socket connection
}

IndexResult ClientProcessingEngine::indexFolder(std::string folderPath) {
    IndexResult result = {0.0, 0};
    // TO-DO get the start time
    // TO-DO crawl the folder path and extrac all file paths
    // TO-DO for each file extract all alphanumeric terms that are larger than 2 characters
    //       and count their frequencies
    // TO-DO increment the total number of bytes read
    // TO-DO for each file prepare an INDEX REQUEST message and send to the server
    //       the document path, the client ID and the word frequencies
    // TO-DO receive for each INDEX REQUEST message an INDEX REPLY message
    // TO-DO get the stop time and calculate the execution time
    // TO-DO return the execution time and the total number of bytes read

    return result;
}

SearchResult ClientProcessingEngine::search(std::vector<std::string> terms) {
    SearchResult result = {0.0, { }};
    // TO-DO get the start time
    // TO-DO prepare a SEARCH REQUEST message that includes the search terms and send it to the server
    // TO-DO receive one or more SEARCH REPLY messages with the results of the search query
    // TO-DO get the stop time and calculate the execution time
    // TO-DO return the execution time and the top 10 documents and frequencies

    return std::move(result);
}

void ClientProcessingEngine::connect(std::string serverIP, std::string serverPort) {
    // TO-DO implement connect to server
    // create a new TCP/IP socket and connect to the server
}

void ClientProcessingEngine::disconnect() {
    // TO-DO implement disconnect from server
    // TO-DO send a QUIT message to the server
    // close the TCP/IP socket
}