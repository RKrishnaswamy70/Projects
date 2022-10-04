/*
 * WebServer.cpp
 *
 *  Created on: Nov 9, 2021
 *      Author: Family
 */

#include <unistd.h>	  // for close()
#include <string.h>   // for strerror()
#include <iostream>
#include <exception>
#include <netinet/in.h>
#include <sys/socket.h>

#include "inc/WebServer.h"

#define MAINSOCKET_PORT 8080

WebServer::WebServer()
{
    int opt = 1;

	// Create the socket.
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == 0) {
		throw std::logic_error("Could not create main socket!");
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

    // This is set true when shutdown() is called.
    m_shutdown = false;
}


WebServer::~WebServer()
{
}


void WebServer::run()
{
	int ok;
	int connectionSocket;
	int addrlen = sizeof(m_address);

	ok = listen(m_socket, 5);
	if (ok < 0) {
		throw std::logic_error("Could not listen at main socket!");
	}

	while (!m_shutdown) {
		connectionSocket = accept(m_socket, (struct sockaddr *)&m_address,
		                          (uint32_t*) &addrlen);
		if (connectionSocket < 0) {
			// Timed out.  Check again.
			continue;
		}

		handleRequest(connectionSocket);
	}

	close(connectionSocket);
}


//bool WebServer::process(const string& path, string& htmlText)
//{
//	std::cout << "Processing path: " << path << std::endl;
//	htmlText = "HTTP/1.1 200 OK\n\nHello World!\n";
//	htmlText += "Path = " + path + "\n";
//	return true;
//}


#define BUFFER_SIZE 1024
void WebServer::handleRequest(int connectionSocket)
{
	int recvLen;

	string htmlText;
	string responseText;
	char buffer[BUFFER_SIZE];

	// Setup the socket to timeout on recv
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(connectionSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	while (true) {
		recvLen = recv(connectionSocket, buffer, BUFFER_SIZE, 0);
		if (recvLen < 0) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				// Timed out.  Redo the loop.  This allows it to
				// check the m_shutdown boolean, to see if this
				// connection is to be shutdown.
				continue;
			} else {
				std::cout << "Receive error: " << strerror(errno) << std::endl
						  << "Terminating connection" << std::endl;
				break;
			}
		} else if (recvLen == 0) {
			// Redo loop to get data
			continue;
		}

		// Now call the virtual function WebServer::process
		buffer[recvLen] = '\0';
		responseText = buffer;
		process(responseText, htmlText);

		if (responseText == "shutdown") {
			std::cout << "Received shutdown" << std::endl;
			m_shutdown = true;
			break;
		}

		// Now send it back to through the connection socket
		recvLen = send(connectionSocket, htmlText.c_str(), htmlText.length() + 1, 0);
		if (recvLen < 0) {
			std::cout << "Send error: " << strerror(errno) << std::endl
					  << "Terminating connection" << std::endl;
		}

		// Request is handled.
		break;
	}

	// Done with this connection.
	close(connectionSocket);
}
