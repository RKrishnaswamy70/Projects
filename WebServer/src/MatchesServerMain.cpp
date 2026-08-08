//============================================================================
// Name        : WebServer.cpp
// Author      : R. Krishnaswamy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "inc/MatchesServer.h"

// Instructions:
// This program is a web server.  So to see the run of the program, you
// will need to start the program, and also start up a web browser (Firefox)
// on an URL.
//
// If the browser and the server are on the same machine:
//   a. In a shell, do:
//         hostname -I
//      The output will be something like:
//         192.168.1.101 192.168.122.1 2600:1700:ce00:e260::33 2600:1700:ce00:e260:1efd:8ff:fe73:d39d
//      The first 4 numbers are the ip-address of the server which will be used
//      by this program.
//   b. Startup this main program
//   c. Suppose the server ip-address in (a) is 192.168.1.101.
//      In Firefox, open web page: http://192.168.1.101:8080/bar
//   d. Play the matches game.
//   e. Terminate the server either by shutting down from the Matches web
//      page by the finish button, terminating in the debugger, or killing
//      the process.
//
// If the browser and the server are on different machines:
//   1. On the server machine, as super-user, do the command:
//         netstat -a | grep :8080
//      There should be no output.  The port 8080 should be unused.
//   2. Then do
//         firewall-cmd --add-port=8080/tcp
//   3. Now do a, b, c, d and e above.
//   4. Now we will need to undo the port addition:
//         firewall-cmd --remove-port=8080/tcp
//
// To see the html text that is produced, see the data file Matches.html.
// The $ServerIpAddr marker in Matches.html will be replaced by the actual
// ip-address of the server.


int main() {
	MatchesServer matchesServer;

	matchesServer.run();

	return 0;
}
