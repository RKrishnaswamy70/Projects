#include "inc/ChatClient.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <exception>
#include <iostream>
#include <string.h> // For strerror()
#include <thread>

#include "Port.h"
#include "MessageData.h"

using namespace std;


ChatClient::ChatClient(const string& name)
: m_name(name), m_socket(0),

  // m_shutdown is true, indicating the socket is not yet
  // initialized.  This blocks the m_receiver thread.
  m_shutdown(true),

  // This constructor starts up the receiver thread,
  // running the runReceiverLoop() method on this object.
  // It will block till m_shutdown is true, indicating
  // that m_socket is ready.
  m_receiver(&ChatClient::runReceiveLoop, this)
{
	int ok;

	// Constructor initializes m_socket and then sets
	// m_shutdown to false and notifies the m_receiver
	// thread to go ahead in the receive loop.
    struct sockaddr_in serverAddr;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0) {
		throw std::logic_error("ChatClient could not create main socket!");
	}

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MAINSOCKET_PORT);

    ok = connect(m_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ok < 0)
    {
		throw std::logic_error("ChatClient could not connect to main socket!");
    }

	// Now setup the socket to timeout on blocking calls, such as accept,
    // connect, read, recv, etc:
    //  https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=supported-timeouts
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	// First send the name to the ChatServer
	ok = send(m_socket, name.c_str(), name.length() + 1, 0);
	if (ok < 0) {
		string msg = "ChatClient error sending name: ";
		msg += strerror(errno);
		throw std::logic_error(msg);
	}

	// Now can start the receiver thread
	{
	    std::unique_lock<std::mutex> lk(m_mutex);

	    m_shutdown = false;
	}
	m_cv.notify_all();
}


ChatClient::~ChatClient()
{
	m_receiver.join();
	close(m_socket);
}


#define BUFFER_SIZE 1024
void ChatClient::runReceiveLoop()
{
    int recvCount;
	char buffer[BUFFER_SIZE] = {0};

	// Block this thread until m_shutdown is false, indicating
	// that the socket is initialized.
	{
	    std::unique_lock<std::mutex> lk(m_mutex);

		// Wait till m_shutdown is false.
		m_cv.wait(lk, [this]()->bool {return m_shutdown == false;});
	}

	// Now m_shutdown is false.  We can run the receive loop
    while (!m_shutdown) {
    	// Receive from the socket, and process it.
    	recvCount = recv(m_socket, buffer, BUFFER_SIZE, 0);
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

		// Now actually received data. Convert the buffer into MessageData and
		// print it out, and then receive data again.
		MessageData msgData(buffer, recvCount);
		const string& from = msgData.getFrom();
		const string& msg = msgData.getMessage();
		std::cout << "\n\tFrom: " << from << std::endl
				  << "\tMessage: " << msg << std::endl;

		if ((from == "server") && (msg == "shutdown")) {
			m_shutdown = true;
			break;
		}
    }
}


void ChatClient::runInputLoop()
{
	int ok;
	string to;
	string message;

	// As this method is called on the same thread as the constructor,
	// we know that the socket is initialized.  So do not need to block
	// till m_shutdown is false.

	while (!m_shutdown) {
		std::cout << "Send to: ";
		getline(std::cin, to);

		// Check if a shutdown came.
		if (m_shutdown) {
			break;
		}

		if (to.empty()) {
			// Invalid message
			continue;
		}

		std::cout << "Message: ";
		getline(std::cin, message);

		// Check if a shutdown came.
		if (m_shutdown) {
			break;
		}

		MessageData msgData(m_name, to, message);
		string msgStr = msgData.getSendString();
		ok = send(m_socket, msgStr.c_str(), msgStr.length() + 1, 0);
		if (ok < 0) {
			std::cout << "Send error: terminating connection, errno" << strerror(errno) << std::endl;
			break;
		}

		// No need to check m_shutdown, will be done by while-statement
	}
}
