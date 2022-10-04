/*
 * ConnectionListener.cpp
 *
 *  Created on: Nov 9, 2021
 *      Author: Family
 *
 * Note: We needed to add to the settings:
 *    To: Project->Properties->C/C++ Build->Settings->GCC C++ Linker->Libraries
 *    Add: pthread
 */
#include <string.h> // for strerror() function
#include <thread>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>

#include "MessageData.h"
#include "inc/ChatServer.h"
#include "ConnectionListener.h"

// Constructor takes a socket, and starts up a
// listener thread listening on that socket.
ConnectionListener::ConnectionListener(const string& clientName,
		                               int connectionSocket,
				                       ChatServer* pChatServer)
: m_clientName(clientName),
  m_connectionSocket(connectionSocket),
  m_pChatServer(pChatServer),

  // This constructor starts up the listener thread,
  // running the run() method on this object.
  m_listener(&ConnectionListener::run, this),

  m_shutdown(false)
{
}


ConnectionListener::~ConnectionListener()
{
	m_listener.join();
}


string ConnectionListener::getClientName() const
{
	return m_clientName;
}


// The ChatServer calls this method of the ConnectionListener of
// the "to" client in the messageData.  The ConnectionListener
// then sends the message to the ChatClient of the "to" client.
void ConnectionListener::send(const MessageData& messageData)
{
	int ok;
	string sendString = messageData.getSendString();

	// Now send it through the connection socket to the associated ChatClient
	ok = ::send(m_connectionSocket, sendString.c_str(), sendString.length() + 1, 0);
	if (ok < 0) {
		std::cout << "Send error: terminating connection, errno" << strerror(errno) << std::endl;
	}
}



// Sets m_shutdown to true, and then waits till shutdown is
// complete.
void ConnectionListener::shutdown()
{
	// This will cause the run() method to eventually stop.
	m_shutdown = true;
}


#define BUFFER_SIZE 1024

// This function is run on the listener thread.
void ConnectionListener::run()
{
	int recvCount;
	char buffer[BUFFER_SIZE];

	// Setup the socket to timeout on recv
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(m_connectionSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	while (!m_shutdown) {
		recvCount = recv(m_connectionSocket, buffer, BUFFER_SIZE, 0);
		if (recvCount < 0) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				// Timed out.  Redo the loop.  This allows it to
				// check the m_shutdown boolean, to see if this
				// connection is to be shutdown.
				continue;
			} else {
				std::cout << "Receive error: " << strerror(errno) << std::endl;
				break;
			}
		} else if (recvCount == 0) {
			// Redo loop to get data
			continue;
		}

		// Now we are ready to call the method to send the message through
		// the ChatServer to its destination.
		MessageData msgData(buffer, recvCount);

		string to = msgData.getTo();
		if (to == "server") {
			// It is a message to the server.  Check the message
			string msg = msgData.getMessage();
			if (msg == "shutdown") {
				// ChatServer sent a shutdown to this listener.
				std::cout << "Received shutdown from " << m_clientName << std::endl;
				m_pChatServer->shutdown();
				break;
			} else if (msg == "bye") {
				// ChatClient sent a bye to this listener, causing
				// only this listener to shutdown.  It is important
				// to do call ChatServer::shutdown so that this
				// ConnectionListener may be removed from its map.
				std::cout << "Received bye from " << m_clientName << std::endl;
				m_pChatServer->shutdown(this);
				break;
			} else {
				std::cout << "Unrecognized message '" << msg
						  << "' from '" << msgData.getFrom()
						  <<"'" << std::endl;
			}
		} else if (to == "*") {
			// Send the message to all clients.
			m_pChatServer->sendAll(msgData);
		} else {
			// It is a message to another client
			// The message needs to be sent to msgData.getTo().
			m_pChatServer->send(msgData);
		}
	}

	// ConnectionListener needs to send a shutdown message to its client
	MessageData shutdownMsg("server", m_clientName, "shutdown");
	send(shutdownMsg);

	// Done with this connection.
	close(m_connectionSocket);
}


