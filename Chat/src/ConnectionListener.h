/*
 * ConnectionListener.h
 *
 *  Created on: Nov 9, 2021
 *      Author: Family
 */

#pragma once

#include <thread>
#include <atomic>
#include <pthread.h>


// Forward declarations
class ChatServer;
class MessageData;

using namespace std;

// This class services a connection.
class ConnectionListener
{
public:
	// Constructor takes a socket, and starts up a
	// listener thread listening on that socket.
	ConnectionListener(const string& clientName,
			           int connectionSocket,
			           ChatServer* pWebServer);

	// Shuts down the listener thread, and destructs.
	~ConnectionListener();

	// To get its client name
	string getClientName() const;

	// To send a message
	void send(const MessageData& messageData);

	// Causes the listener thread to exit.
	void shutdown();

private:
	// This is the function that is run by the listener thread
	void run();

	// Client name
	string m_clientName;

	// The socket used by this connection
	int m_connectionSocket;

	// The web server
	ChatServer* m_pChatServer;

	// The listening thread
	std::thread m_listener;

	std::atomic<bool> m_shutdown;
};
