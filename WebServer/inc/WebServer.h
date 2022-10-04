// The class WebServer encapsulates the functionality of a
// web server that accepts requests from clients for connections.
//
// Instructions:
//   1. Startup WebServerMain
//   2. In Firefox, open web page http://localhost:8080/bar

#pragma once

#include <list>
#include <string>
#include <netinet/in.h>

using namespace std;

class WebServer
{
public:
	// Default constructor.  Uses a fixed port for the
	// main socket.
	WebServer();

	// Virtual destructor
	virtual
	~WebServer();

	// To run the web server.
	void run();

	// This is the function that should perform the service,
	// interpreting a path from a client, and filling in the
	// string containing the path of the html file to display.  It should
	// return false if the path is unrecognized.
	//
	// There is a default interpretation, which simply copies
	// the responseText into the htmlText.  Subclasses can override
	// this function.
	virtual
	bool process(const string& responseText, string& htmlText);

private:
	// This is the main socket that is used to listen for
	// connection.
	int m_socket;

	// This is the socket address
	sockaddr_in m_address;

	// This is to shutdown connections
	bool m_shutdown;

	// To handle a request from a socket
	void handleRequest(int connectionSocket);
};
