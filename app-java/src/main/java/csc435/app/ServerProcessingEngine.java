package csc435.app;

import java.util.ArrayList;

public class ServerProcessingEngine {
    private IndexStore store;
    // TO-DO keep track of the Dispatcher thread
    // TO-DO keep track of the Index Worker threads
    // TO-DO keep track of the clients information

    public ServerProcessingEngine(IndexStore store) {
        this.store = store;
    }

    public void initialize(int serverPort) {
        // TO-DO create and start the Dispatcher thread
    }

    public void spawnWorker() {
        // TO-DO create and start a new Index Worker thread
    }

    public void shutdown() {
        // TO-DO signal the Dispatcher thread to shutdown
        // TO-DO join the Dispatcher and Index Worker threads
    }

    public ArrayList<String> getConnectedClients() {
        // TO-DO return the connected clients information
        return new ArrayList<String>();
    }
}
