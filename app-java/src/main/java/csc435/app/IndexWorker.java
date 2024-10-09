package csc435.app;

public class IndexWorker implements Runnable {
    private IndexStore store;

    public IndexWorker(IndexStore store) {
        this.store = store;
    }
    
    @Override
    public void run() {
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
}
