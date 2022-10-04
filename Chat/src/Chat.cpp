//============================================================================
// Name        : Chat.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "inc/ChatServer.h"
#include "inc/ChatClient.h"
using namespace std;
/**************************************************
 * This is a simple Chat application.
 *
 * 1. To run Chat Server enter the following at the shell from
 *    the Chat project directory:
 *       ./Debug/Chat server
 * 2. For each Chat client enter the following in another
 *    shell from the Chat project
 *       ./Debug/Chat <name-of-client>
 *    The <name-of-client> can be anything, say aaa.
 * 3. Then enter messages supplying the To string and Message string.
 * 4. Any client can enter a message to all by supplying "*" as the
 *    To string.
 * 5. Any client can terminate self by setting To as "server" and
 *    Message as "bye".  Will need to supply another <CR> to terminate.
 * 6. Any client can initiate shutdown of all (including server) by
 *    setting To as "server" and Message as "shutdown".
 *
 * Design:
 * Main program is Chat.  It runs either an instance of ChatServer,
 * or an instance of ChatClient.
 *
 * ChatServer process:
 * a. ChatServer::run() runs after construction.  It starts listening on
 *    a socket at a specific port (from Port.h).  It is accepting
 *    connection requests.
 * b. On a connection request, ChatServer connects to a socket, and creates
 *    a ConnectionListener instance.  The listener runs a thread listening
 *    for connections, and sending them to the appropriate destination
 *    listener.
 * c. ChatServer has a map from client-names to ConnectionListeners.
 * d. ChatServer destructor stops all listener threads, deletes
 *    ConnectionListeners and shuts down.
 *
 * ChatClient process:
 * a. ChatClient constructor creates a thread to listen for messages.
 *    This thread runs runReceiveLoop().
 * b. ChatClient::runReceiveLoop() runs on construction waiting for
 *    messages sent from ChatServer, and prints them out.
 * c. ChatClient has another thread, an input thread, waiting for
 *    user input.
 **************************************************/

int main(int argc, char** argv) {
	if (argc == 1) {
		return 0;
	}

	string arg1 = argv[1];
	if (arg1 == "server") {
		// arg1 is the special name "server", to startup the ChatServer
		ChatServer chatServer;

		chatServer.run();
	} else {
		// arg1 is the name of the client, to startup the ChatClient
		ChatClient chatClient(arg1);

		chatClient.runInputLoop();
	}

	return 0;
}
