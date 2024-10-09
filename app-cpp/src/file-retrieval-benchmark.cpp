#include <iostream>
#include <vector>
#include <string>

void runWorker()
{
    // TO-DO connect the ClientProcessingEngine to the server
    // TO-DO index the dataset
}

int main(int argc, char** argv)
{
    std::string serverIP;
    std::string serverPort;
    int numberOfClients;
    std::vector<std::string> clientsDatasetPath;

    // TO-DO extract the arguments from args
    // TO-DO measure the execution start time
    // TO-DO create ClientProcessingEngine objects equal to the number of clients
    // TO-DO create and start benchmark worker threads equal to the number of clients
    // TO-DO join the benchmark worker threads
    // TO-DO measure the execution stop time and print the performance
    // TO-DO run search queries on the first client (benchmark worker thread number 1)
    // TO-DO disconnect all clients (all benchmakr worker threads)

    return 0;
}