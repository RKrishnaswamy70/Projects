/*
 * ChatServer.cpp
 *
 *  Created on: Nov 9, 2021
 *      Author: Family
 */

#include <iostream>
#include <exception>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Port.h"
#include "MessageData.h"
#include "inc/ChatServer.h"
#include "ConnectionListener.h"

ChatServer::ChatServer()
{
    int opt = 1;

	// Create the socket.
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0) {
		throw std::logic_error("ChatServer could not create main socket!");
	}

	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	                                                  &opt, sizeof(opt))) {
		throw std::logic_error("Could not set main socket options!");
	}

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(MAINSOCKET_PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(m_socket, (struct sockaddr *)&m_address,
                                 sizeof(m_address))<0)
    {
    	throw std::logic_error("Could not set bind main socket!");
    }

	// Now setup the socket to timeout on blocking calls, such as accept,
    // connect, read, recv, etc:
    //  https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=supported-timeouts
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	if (listen(m_socket, 5) < 0) {
		throw std::logic_error("Could not listen at main socket!");
	}

    // This is set true when shutdown() is called.
    m_shutdown = false;
}


ChatServer::~ChatServer()
{
}


#define BUFFER_SIZE 1024
void ChatServer::run()
{
    int recvCount;
    string clientName;
	int connectionSocket;
	char buffer[BUFFER_SIZE] = {0};
	int addrlen = sizeof(m_address);
	ConnectionListener* pConnectionListener;

	while (!m_shutdown) {
		connectionSocket = accept(m_socket, (struct sockaddr *)&m_address,
		                          (uint32_t*) &addrlen);
		if (connectionSocket < 0) {
			// Check again ... maybe m_shutdown is false.
			continue;
		}

		// Now receive the name of the ChatClient using the newly minted
		// connectionSocket.
		recvCount = recv(connectionSocket, buffer, BUFFER_SIZE, 0);
		if (recvCount <= 0) {
			std::cout << "Could not get name of connection" << std::endl;
			continue;
		}

    	clientName = buffer;

    	pConnectionListener = new ConnectionListener(clientName, connectionSocket, this);
		m_connectionListeners[clientName] = pConnectionListener;
	}

	// Now shutdown all connections
	for (auto const& entry : m_connectionListeners)
	{
		pConnectionListener = entry.second;
		pConnectionListener->shutdown();
		delete pConnectionListener;

		// No need to erase it from the m_connectionListeners, as the
		// map is going to be destroyed.
	}
}


// To send a message
void ChatServer::send(const MessageData& messageData)
{
	map<string, ConnectionListener*>::iterator iter =
			m_connectionListeners.find(messageData.getTo());
	if (iter == m_connectionListeners.end()) {
		// Invalid call.  Ignore.
		std::cout << "ChatServer received message with unknown destination: "
				  << messageData.getTo() << std::endl;
		return;
	}

	// Found the "to" ConnectionListener.  It will send the message
	// to the "to" ChatClient.
	iter->second->send(messageData);
}


// To send a message to all clients.
void ChatServer::sendAll(const MessageData& messageData)
{
	const string& from = messageData.getFrom();
	const string& message = messageData.getMessage();
	ConnectionListener* pConnectionListener;

	for (auto const& entry : m_connectionListeners)
	{
		const string& to = entry.first;

		// Skip sending message to the originator of the message.
		if (to == from) {
			continue;
		}

		MessageData msgData(from, to, message);

		pConnectionListener = entry.second;
		pConnectionListener->send(msgData);
	}
}


void ChatServer::shutdown(ConnectionListener* pConnectionListener)
{
	string clientName = pConnectionListener->getClientName();
	std::map<string, ConnectionListener*>::iterator iter =
			m_connectionListeners.find(clientName);
	if (iter == m_connectionListeners.end()) {
		// Some error.  Ignore
		return;
	}

	pConnectionListener->shutdown();
	m_connectionListeners.erase(iter);
}

void ChatServer::shutdown()
{
	// Causes the run loop to shutdown.
	m_shutdown = true;
}





