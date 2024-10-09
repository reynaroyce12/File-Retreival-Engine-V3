package csc435.app;


public class Dispatcher implements Runnable {
    private ServerProcessingEngine engine;

    public Dispatcher(ServerProcessingEngine engine) {
        this.engine = engine;
    }
    
    @Override
    public void run() {
        // TO-DO create a TCP/IP socket and listen for new connections
        // TO-DO When new connection comes through create a new Index Worker thread for the new connection
        // TO-DO Use the engine spawnWorker method to create a new Index Worker thread
    }
}
