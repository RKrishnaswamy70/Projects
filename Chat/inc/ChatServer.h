// The class ChatServer encapsulates the functionality of a
// web server that accepts requests from clients for connections,
// and allows the clients to chat with one another.

#include <map>
#include <atomic>
#include <string>
#include <netinet/in.h>

#pragma once

using namespace std;

// Forward declarations
class MessageData;
class ConnectionListener;

class ChatServer
{
public:
	// Default constructor.  Uses a fixed port for the
	// main socket.
	ChatServer();

	// Virtual destructor
	virtual
	~ChatServer();

	// To run the web server.
	void run();

	// To shutdown one connection
	void shutdown(ConnectionListener* pConnectionListener);

	// To send a message
	void send(const MessageData& messageData);

	// To send a message to all clients
	void sendAll(const MessageData& messageData);

	// To shutdown this WebServer and all its connections.
	void shutdown();

private:
	// This is the main socket that is used to listen for
	// connection.
	int m_socket;

	// This is the socket address
	sockaddr_in m_address;

	// The set of listeners
	std::map<string, ConnectionListener*> m_connectionListeners;

	// This is to shutdown connections
	std::atomic<bool> m_shutdown;
};
