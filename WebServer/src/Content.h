/*
 * Content.h
 *
 *  Created on: Feb 17, 2022
 *      Author: Family
 */

#pragma once

#include <string>
#include <map>

using namespace std;

class Content
{
public:
	Content(const string& responseText);

	// Returns "?" if undefined.
	string getValue(const string& key);

private:
	// A map that is constructed from the response text.
	// The keys are the names of page buttons and fields
	// that the user provides values for, and the values
	// are the user provided value for each key.
	map<string, string> contentMap;
};




