package csc435.app;

import java.util.ArrayList;

class IndexResult {
    public double executionTime;
    public long totalBytesRead;

    public IndexResult(double executionTime, long totalBytesRead) {
        this.executionTime = executionTime;
        this.totalBytesRead = totalBytesRead;
    }
}

class DocPathFreqPair {
    public String documentPath;
    public long wordFrequency;

    public DocPathFreqPair(String documentPath, long wordFrequency) {
        this.documentPath = documentPath;
        this.wordFrequency = wordFrequency;
    }
}

class SearchResult {
    public double excutionTime;
    public ArrayList<DocPathFreqPair> documentFrequencies;

    public SearchResult(double executionTime, ArrayList<DocPathFreqPair> documentFrequencies) {
        this.excutionTime = executionTime;
        this.documentFrequencies = documentFrequencies;
    }
}

public class ClientProcessingEngine {
    // TO-DO keep track of the connection (socket)

    public ClientProcessingEngine() { }

    public IndexResult indexFiles(String folderPath) {
        IndexResult result = new IndexResult(0.0, 0);
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
    
    public SearchResult searchFiles(ArrayList<String> terms) {
        SearchResult result = new SearchResult(0.0, new ArrayList<DocPathFreqPair>());
        // TO-DO get the start time
        // TO-DO prepare a SEARCH REQUEST message that includes the search terms and send it to the server
        // TO-DO receive one or more SEARCH REPLY messages with the results of the search query
        // TO-DO get the stop time and calculate the execution time
        // TO-DO return the execution time and the top 10 documents and frequencies

        return result;
    }

    public void connect(String serverIP, String serverPort) {
        // TO-DO implement connect to server
        // create a new TCP/IP socket and connect to the server
    }

    public void disconnect() {
        // TO-DO implement disconnect from server
        // TO-DO send a QUIT message to the server
        // close the TCP/IP socket
    }
}
