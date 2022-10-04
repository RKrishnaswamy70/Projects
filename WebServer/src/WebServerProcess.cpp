/*
 * WebServerProcess.cpp
 *
 *  Created on: Nov 12, 2021
 *      Author: Family
 */

#include "inc/WebServer.h"

#include <map>
#include <sstream>
#include <iostream>

#include "Content.h"

using namespace std;

// Instructions:
//   1. Startup WebServerMain
//   2. In Firefox, open web page http://localhost:8080/bar

/****************************************************
This is a sample of the text produced.  The "value1" field is
actually the number of matches.  The "value3" radio-button
is the user play choice.

<!DOCTYPE HTML//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html>
<head><title>Test CGIcc form</title></head>
<body bgcolor="#cccccc" text="#000000">
<h2>Test CGIcc form</h2>
<p>
<form action="/response" method="post">
	Value 1 :
	<input type="text" name="value1">
	<p>
	Value 2 :
	<select name="value2">
	   <option value="option1">Option 1
	   <option value="option2">Option 2
	   <option value="option3">Option 3
	</select>
	<P>
	Value 3 :
	<input type="radio" name="value3" value="button1" checked="checked">Button1
	<input type="radio" name="value3" value="button2">Button2
	<input type="hidden" name="value4" value="data4">
	<p>
	<input type="submit" value="Submit">
</form>
</body>
</html>
****************************************************/

static string s_htmlText =
		"<html>\n"
		"<head><title>Match Game</title></head>\n"
		"<body bgcolor=\"#cccccc\" text=\"#000000\">\n"
		"<h2>Match Game</h2>\n"
		"<p>\n"
	//	"<form method=\"post\" action=\"/cgi-bin/testcgi\">\n"
		"<form action=\"/response\" method=\"post\">\n"
		"	Matches :\n"
		"	<input type=\"text\" name=\"matches\" value=\"$Matches\">\n"
		"	<p>\n"
		"	Player  :\n"
		"	<select name=\"player\">\n"
		"	   <option value=\"option1\">1 Match\n"
		"	   <option value=\"option2\">2 Matches\n"
		"	   <option value=\"option3\">3 Matches\n"
		"	   <option value=\"option4\">4 Matches\n"
		"	</select>\n"
		"	<P>\n"
		"	Value 3 :\n"
		"	<input type=\"radio\" name=\"value3\" value=\"button1\" checked=\"checked\">Button1\n"
		"	<input type=\"radio\" name=\"value3\" value=\"button2\">Button2\n"
		"	<input type=\"hidden\" name=\"value4\" value=\"data4\">\n"
		"	<p>\n"
		"   $Play\n"
		"   <p>\n"
		"	<input type=\"submit\" value=\"Submit\">\n"
		"</form>\n"
		"</body>\n"
		"</html>\n";

static string getHtmlText(const string& matchesValue, const string& playValue)
{
	string matchesKey = "$Matches";
	size_t matchesIndex = s_htmlText.find(matchesKey);
	if (matchesIndex == string::npos) {
		return s_htmlText;
	}

	string playKey = "$Play";
	size_t playIndex = s_htmlText.find(playKey);
	if (playIndex == string::npos) {
		return s_htmlText;
	}

	size_t afterMatches = matchesIndex + matchesKey.length();
	string result = s_htmlText.substr(0, matchesIndex) +
			        matchesValue +
		            s_htmlText.substr(afterMatches, playIndex - afterMatches) +
					playValue +
					s_htmlText.substr(playIndex + playKey.length());

	return result;
}


static void formExample(const string& responseText, string& htmlText)
{
	Content content(responseText);
	string matchesStr = content.getValue("matches");
	string playerStr = content.getValue("player");
	string value3Str = content.getValue("value3");

	stringstream strm;

	// Setup
	strm << "HTTP/1.1 200 OK\n\n"
		 << "<!DOCTYPE HTML//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
		 << endl;

	if (matchesStr == "?") {
		// Initial screen.  The initial number of matches is 25,
		// and the play value is the empty string as the computer
		// has not yet played.
		strm << getHtmlText("25", "");
		htmlText = strm.str();
		return;
	}

//	std::cout << "matches = " << matchesStr << std::endl;
//	std::cout << "player = " << playerStr << std::endl;
//	std::cout << "value3 = " << value3Str << std::endl;

	string userChoice;
	string cmpChoice;

	int matches = std::stoi(matchesStr);

	// User play, pick up the digit in "option1", "option2", etc
	// and take that as the user's choice.
	string playerTake = playerStr.substr(playerStr.length() - 1);
	int take = std::stoi(playerTake);
	if (matches > 0) {
		matches -= take;
		userChoice = to_string(take);
	}

	// Now computer play
	string newMatchesStr;
	if (matches > 0) {
		take = matches % 5;

		if (take == 0) {
			take = 1;
		}

		matches -= take;
		newMatchesStr = to_string(matches);
		cmpChoice = to_string(take);
	}

	string play = "From ";
	play += matchesStr;
	play += " user takes ";
	play += userChoice;
	play += ", computer takes ";
	play += cmpChoice;
	play += " leaving ";
	play += newMatchesStr;


	strm << getHtmlText(newMatchesStr, play);
	htmlText = strm.str();
}


// This is a simple cgicc process method for WebServer.
// It can be overridden by a subclass.
bool WebServer::process(const string& responseText, string& htmlText)
{
	try {
		formExample(responseText, htmlText);
	} catch(const exception& e) {
		// handle error condition
	}

	return true;
}



