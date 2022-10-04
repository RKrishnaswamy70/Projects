/*
 * WebServer.cpp
 *
 *  Created on: Nov 9, 2021
 *      Author: Family
 */

#include <cerrno>
#include <unistd.h>	  // for close()
#include <string.h>   // for strerror()
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>       // for gethostbyname()
#include <arpa/inet.h>   // for inet_ntoa()


#include "inc/WebServer.h"

#define MAINSOCKET_PORT 8080


static string getHostIpAddress()
{
	string hostURL = "?.?.?.?";

    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == -1) {
        return hostURL;
    }

    struct hostent *hostEntry = gethostbyname(hostName);
    if (hostEntry == NULL) {
        return hostURL;
    }

    char *ipAddress = inet_ntoa(*(struct in_addr*)hostEntry->h_addr_list[0]);
    std::cout << hostName << ", " << ipAddress << std::endl;

    return string(ipAddress);
}


static string getHostURL(const string& ipAddr)
{
	string hostURL;

    hostURL = "http://";
    hostURL += ipAddr;
    hostURL += ":";
    hostURL += std::to_string(MAINSOCKET_PORT);
    hostURL += "/DontCare";

    return hostURL;
}


WebServer::WebServer()
{
    int opt = 1;

    string serverIpAddr = getHostIpAddress();

    // Save the server URL. It will be needed to modify the marker
    // $ServerURL in the html file into the full server url.
    m_serverURL = getHostURL(serverIpAddr);

    std::cout << "Open browser to web page: "
			  << m_serverURL
			  << std::endl;

    // Create the listening socket.
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
        string errorMsg = "Could not create main socket! ";
        errorMsg += strerror(errno);
        throw std::logic_error(errorMsg);
    }

    // We cannot set socket options SO_REUSEADDR and SO_REUSEPORT together
    // using bitwise-or.  This is because these options are unique ids
    // (e.g. 2 and 15), and not bit-masks that can be or'ed.  So the
    // bitwise-or results in some totally new unrecognized id.

    // Set SO_REUSEADDR
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        // Capture errno immediately before any other function calls modify it
        string errorMsg = "Could not set socket option SO_REUSEADDR! ";
        errorMsg += strerror(errno);
        throw std::logic_error(errorMsg);
    }

    // Set SO_REUSEPORT
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        // Capture errno immediately before any other function calls modify it
        string errorMsg = "Could not set socket option SO_REUSEPORT! ";
        errorMsg += strerror(errno);
        throw std::logic_error(errorMsg);
    }

    m_address.sin_family = AF_INET;
    // The following setting is a 32-bit integer for an IPV4 address.
    // It could be INADDR_ANY, but we set it to the actual ip-address
    // of the server which was reported at startup in the path for
    // client browsers to connect to.
    m_address.sin_addr.s_addr = inet_addr(serverIpAddr.c_str());
    m_address.sin_port = htons(MAINSOCKET_PORT);

    // Forcefully attaching socket to the MAINSOCKET_PORT
    if (bind(m_socket, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        // Capture errno immediately before any other function calls modify it
        string errorMsg = "Could not bind main socket! ";
        errorMsg += strerror(errno);
        throw std::logic_error(errorMsg);
    }

    // This is set true when shutdown() is called.
    m_shutdown = false;
}


WebServer::~WebServer()
{
	// Close the listening socket.
	close (m_socket);
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

	// Initialize a timeout for the connection socket
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	while (!m_shutdown) {
		connectionSocket = accept(m_socket, (struct sockaddr *)&m_address,
		                          (uint32_t*) &addrlen);
		if (connectionSocket < 0) {
			// Timed out.  Check again.
			continue;
		}

		// Setup the timeout for the connectionSocket
	    if (setsockopt(connectionSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
	        // Capture errno immediately before any other function calls modify it
	        string errorMsg = "ChatServer could not set socket option SO_RCVTIME0! ";
	        errorMsg += strerror(errno);
	        throw std::logic_error(errorMsg);
	    }

		handleRequest(connectionSocket);

		close(connectionSocket);
	}

	// Debug output
	std::cout << "Matches Server shutting down" << std::endl;
}


#define BUFFER_SIZE 1024
void WebServer::handleRequest(int connectionSocket)
{
	int recvLen;

	string serverText;
	string clientText;
	char buffer[BUFFER_SIZE];

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
		clientText = buffer;

		// Debug output
		std::cout << "Debug ======== Start client text" << std::endl
				  << clientText << std::endl
				  << "Debug ======== End client text" << std::endl
				  << std::endl;

		if (clientText.find("GET") == 0) {
			serverText = doGet();
		} else {
			serverText = doPost(clientText);
		}

		// Debug output
		std::cout << "Debug ======== Start server text" << std::endl
				  << serverText << std::endl
				  << "Debug ======== End server text" << std::endl
				  << std::endl;

		// Now send it back to through the connection socket
		recvLen = send(connectionSocket, serverText.c_str(), serverText.length(), 0);
		if (recvLen < 0) {
			// Debug output
			std::cout << "Send error: " << strerror(errno) << std::endl
					  << "Terminating connection" << std::endl;
		}

		// Request is handled.
		break;
	}

}


WebServer::ParamsMap::ParamsMap(const string& jsonStr)
{
	// Very simple scanner scanning for: { "xxx" : "ppp", "yyy" : "qqq" }
	char ch;
	size_t i,j;
	string domainValue;
	string rangeValue;
	size_t len = jsonStr.length();

	// Initially we are expecting a domain value
	bool expectingRangeValue = false;
	for (i = 0; i < len; i++) {
		ch = jsonStr[i];
		switch (ch) {
		case '{':
		case ',':
			break;

		case ':':
			expectingRangeValue = true;
			break;

		case '"':
			// This must be an opening quote.  Move i past it.
			i++;

			// Scan for a value.  Search for the closing quote.
			for (j = i + 1; jsonStr[j] != '"'; j++)
				// Keep looping
				;

			// At this point, jsonStr[i..j-1] is the string.
			if (expectingRangeValue) {
				// Note that this rangeValue may have newlines in it.
				// That is ok.  They can be saved as a value with newlines
				rangeValue = jsonStr.substr(i,j-i);

				// Can turn off expectingRangeValue.
				expectingRangeValue = false;

				// We have found a domain value mapped to a range value.
				// The domainValue has already been encountered.
				this->insert(std::make_pair(domainValue, rangeValue));

				// Null out the domain and range values
				domainValue.clear();
				rangeValue.clear();
			} else {
				domainValue = jsonStr.substr(i, j-i);
			}

			// j is at the closing quote.  So move i to point to that.
			// It will then come to the for-loop which will increment
			// i, and then test the next char after the closing quote.
			i = j;
			break;

		case '}':
			// Done
			break;

		default:
			// keep looping.
			;
		} // end of switch
	} // end of for-loop
} // end of constructor


string WebServer::ParamsMap::toJSONString() const
{
	char ch;
	bool needsComma;
	string jsonStr;
	stringstream strm;
	stringstream strmForRangeValue;
	// Notice we need const_iterator since the function is const.
	ParamsMap::const_iterator iter;
	ParamsMap::const_iterator endIter ;
	string domainValue;
	string rangeValue;
	size_t rangeLen;

	strm.str("");
	strm << "{ ";
	needsComma = false;
	endIter = ParamsMap::end();
	for (iter = ParamsMap::begin(); iter != endIter; iter++) {
		if (needsComma) {
			// Add a comma before adding this new entry.
			strm << ", ";
		}

		domainValue = iter->first;
		rangeValue = iter->second;
		rangeLen = rangeValue.length();

		// If the rangeValue has newlines in it, they need to be escaped.
		// We can assume that the domainValue, being just a field name,
		// has no newlines or double-quotes.  Start off by clearing
		// the stream for rangeValue.
		strmForRangeValue.str("");
		for (size_t i = 0; i < rangeLen; i++) {
			ch = rangeValue[i];
			if (ch == '\n') {
				// Replace newline by \n
				strmForRangeValue << "\\n";
			} else {
				strmForRangeValue << ch;
			}
		}

		// Now update rangeValue to the new one with newline correction.
		rangeValue = strmForRangeValue.str();

		// json format requires domain-value and range-value to be quoted.
		// Put "<domain-value>" : "<range-value>" into the stream.
		strm << "\""
			 << domainValue
			 << "\" : "
			 << "\""
			 << rangeValue
			 << "\"";

		// Set needsComma to true in case another entry is added.
		needsComma = true;
	}

	// The closing brace
	strm << "}";

	// Now we can get the json string.
	jsonStr = strm.str();

	return jsonStr;
}


// A utility function to read the html text from the file, and substitute
// the server url at the $ServerURL marker.
string getHtmlTextFromFile(const string& filePath, const string& serverURL)
{
	std::ifstream input(filePath);
	if (!input.is_open()) {
		std::cerr << "Unable to open "
				  << filePath;
		return string();
	}

	// We now transfer the text file into the stream.  However, we
	// need to replace the marker "$ServerURL" in the text by
	// m_serverURL.  So we need to check for this marker in the
	// input.
	char ch;
	stringstream strm;
	string serverURLMarker = "$ServerURL";
	while (input.get(ch)) {
		if (ch == '$') {
			// Check the next few chars to see if it is the marker
			// expecting the server-url.  If so, we will need
			// to insert in m_serverURL instead.
			size_t i;
			size_t len = serverURLMarker.length();
			string word = "$";
			// Note we have already seen the $.  So start with i=1.
			for (i = 1; i < len; i++) {
				if (input.get(ch)) {
					word += ch;
					if (ch != serverURLMarker[i]) {
						// Resume the outer loop
						break;
					}
				} else {
					// Got a prefix of the serverURLMarker, and then
					// ran out of text in the file.  Resume the outer loop.
					break;
				}
			}

			// If we got through the entire loop (when i==len), then
			// we need to put serverURL into the stream.  Otherwise,
			// we put word into the stream.
			if (i == len) {
				strm << serverURL;
			} else {
				strm << word;
			}
		} else {
			strm << ch;
		}
	}

	return strm.str();
}


string WebServer::doGet()
{
	// Call the virtual function to get the initial file path.
	const string filePath = getInitialHTMLFilePath();

	// Prepend content header to html text
	string htmlText = "HTTP/1.1 200 OK\n";
	htmlText += getHtmlTextFromFile(filePath, m_serverURL);

	return htmlText;
}


string WebServer::doPost(const string& requestStr)
{
	// The json part of the request string is after the "{"
	size_t idx;
	string requestJSONStr;

	// Chop to the suffix containing the json request
	idx = requestStr.find('{');
    if (idx < 0) {
        idx = requestStr.find('[');
    }

    requestJSONStr = requestStr.substr(idx);

    // Convert it to a ParamsMap
    ParamsMap requestParams(requestJSONStr);

    // This function has the server logic
    ParamsMap responseParams;
    process(requestParams, responseParams);

    // Convert responseStr to a dictionary
    string responseJSONStr = responseParams.toJSONString();

    return responseJSONStr;
}


void WebServer::shutdown()
{
	m_shutdown = true;
}


// To get the initial HTML file path.  This should be overridden by
// the subclass
string WebServer::getInitialHTMLFilePath() const
{
	return "/home/Family/Projects/WebServer/src/WebServerDefault.html";
}


// This is a simple cgicc process method for WebServer.
// It can be overridden by a subclass.
void WebServer::process(const ParamsMap& requestParams, ParamsMap& responseParams)
{
	return;
}
