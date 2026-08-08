// The class WebServer encapsulates the functionality of a
// web server that accepts requests from clients for connections.
//
// Instructions:
//   1. Startup WebServerMain
//   2. In Firefox, open web page http://localhost:8080/bar

#pragma once

#include <map>
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

protected:
	// To shutdown the server.
	void shutdown();

	// The ParamsMap is a string to string mapping names of html
	// elements to their values.
	class ParamsMap : public map<string, string>
	{
	public:
		// Default constructor
		ParamsMap() {}

		// Argument is a json string
		ParamsMap(const string& jsonStr);

		// Returns a json string
		string toJSONString() const;
	};

	// To get the initial HTML file path.  This should be overridden by
	// the subclass
	virtual
	string getInitialHTMLFilePath() const;

	// To process requestParams and compute the responseParams.  It is to
	// be overridden in the server subclass.
	virtual
	void process(const ParamsMap& requestParams, ParamsMap& responseParams);

private:
	// The server url with port, e.g. http://192.168.1.64:8080/DontCare
	string m_serverURL;

	// This is the main socket that is used to listen for
	// connection.
	int m_socket;

	// This is the socket address
	sockaddr_in m_address;

	// This is to shutdown connections
	bool m_shutdown;

	// To handle a request from a socket
	void handleRequest(int connectionSocket);

	// Process a GET request
	string doGet();

	// Process a POST request
	string doPost(const string& requestStr);

};
