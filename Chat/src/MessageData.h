/*
 * MessageData.h
 *
 *  Created on: Nov 17, 2021
 *      Author: Family
 */

#pragma once

#include <string>

using namespace std;

class MessageData
{
public:
	MessageData(const string& from,
			    const string& to,
				const string& message)
	: m_from(from), m_to(to), m_message(message)
	{}

	MessageData(const char* buffer, size_t bufferLen);

	// To get a single string encoding the message data, that
	// can then be sent.
	string getSendString() const;

	// Get the data values
	const string& getFrom() const;
	const string& getTo() const;
	const string& getMessage() const;


private:
	// Data members
	string m_from;
	string m_to;
	string m_message;
};



