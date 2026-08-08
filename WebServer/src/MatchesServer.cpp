/*
 * WebServerProcess.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: Family
 */

#include "inc/MatchesServer.h"

#include <map>
#include <sstream>
#include <iostream>

using namespace std;

MatchesServer::MatchesServer()
{
	// Nothing to do
}


MatchesServer::~MatchesServer()
{
	// Nothing to do
}


string MatchesServer::getInitialHTMLFilePath() const
{
	return "/home/Family/Projects/WebServer/src/MatchesWeb_JS_WebServer.html";
}


void MatchesServer::process
         (const ParamsMap& requestParams, ParamsMap& responseParams)
{
	int newTaken;
	string displayStr;
	// Note, we cannot use requestParams["taken"] since it is a const argument.
    string takenStr = requestParams.at("taken");
    string matchesRemainingStr = requestParams.at("matchesRemaining");

    int matchesRemaining = stoi(matchesRemainingStr);
    int taken = std::stoi(takenStr);

	// Below is the game logic
    if (matchesRemaining < taken) {
    	displayStr = "Sorry, cannot take ";
    	displayStr += takenStr;
    	displayStr += " from ";
    	displayStr += matchesRemainingStr;
    	displayStr += "\n";
    	// matchesRemaining is unchanged
    } else if (matchesRemaining == taken) {
    	// User won.
    	displayStr = "Congratulations! You win!!\n";
    	matchesRemaining = 0;
    } else {
		// Decrement matches remaining.
    	matchesRemaining -= taken;
    	// User play is remainder modulo 5 if possible.
    	newTaken = matchesRemaining % 5;
    	if (newTaken == 0) {
    		// User made winning move.  Take lowest possible, hoping for user mistake!
    		newTaken = 1;
    	}

    	matchesRemaining -= newTaken;
    	matchesRemainingStr = std::to_string(matchesRemaining);

    	displayStr = "You took ";
    	displayStr += takenStr;
    	displayStr += ".  I take ";
    	displayStr += std::to_string(newTaken);
    	displayStr += " leaving ";
    	displayStr += matchesRemainingStr;
    	displayStr += ".\n";

    	if (matchesRemaining == 0) {
            // Server won.
            displayStr += "I win!\n";
    	}
    }

	responseParams["display"] = displayStr;
	responseParams["matchesRemaining"] = matchesRemainingStr;
}
