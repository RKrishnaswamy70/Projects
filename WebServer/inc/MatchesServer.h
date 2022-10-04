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

#include "WebServer.h"
using namespace std;

class MatchesServer : public WebServer
{
public:
	// Default constructor.
	MatchesServer();

	// Virtual destructor
	virtual
	~MatchesServer();

private:
	// To get the initial HTML file path.
	// Overrides the WebServer function.
	virtual
	string getInitialHTMLFilePath() const;

	// To process requestParams and compute the responseParams.
	// Overrides the WebServer function.
	virtual
	void process(const ParamsMap& requestParams, ParamsMap& responseParams);

};
