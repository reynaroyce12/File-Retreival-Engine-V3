package csc435.app;

public class FileRetrievalClient
{
    public static void main(String[] args)
    {
        ClientProcessingEngine engine = new ClientProcessingEngine();
        ClientAppInterface appInterface = new ClientAppInterface(engine);
        
        // read commands from the user
        appInterface.readCommands();
    }
}
