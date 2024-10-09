package csc435.app;

import java.lang.System;
import java.util.Scanner;
import java.util.ArrayList;

public class ServerAppInterface {
    private ServerProcessingEngine engine;

    public ServerAppInterface(ServerProcessingEngine engine) {
        this.engine = engine;
    }

    public void readCommands() {
        // TO-DO implement the read commands method
        Scanner sc = new Scanner(System.in);
        String command;
        
        while (true) {
            System.out.print("> ");
            
            // read from command line
            command = sc.next();

            // if the command is quit, terminate the program       
            if (command.compareTo("quit") == 0) {
                engine.shutdown();
                break;
            }

            // if the command begins with list, list all the connected clients
            if (command.length() >= 4 && command.substring(0, 4).compareTo("list") == 0) {
                // TO-DO call the getConnectedClients method from the server to retrieve the clients information
                // TO-DO print the clients information
                ArrayList<String> clientsInformation = engine.getConnectedClients();
                continue;
            }

            System.out.println("unrecognized command!");
        }

        sc.close();
    }
}
