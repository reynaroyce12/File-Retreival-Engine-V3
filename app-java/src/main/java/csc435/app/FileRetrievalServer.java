package csc435.app;

public class FileRetrievalServer
{
    public static void main( String[] args )
    {
        // TO-DO change server port to a non-privileged port from args[0]
        int serverPort = 1;

        IndexStore store = new IndexStore();
        ServerProcessingEngine engine = new ServerProcessingEngine(store);
        ServerAppInterface appInterface = new ServerAppInterface(engine);
        
        // create a thread that creates and server TCP/IP socket and listenes to connections
        engine.initialize(serverPort);

        // read commands from the user
        appInterface.readCommands();
    }
}
