/*
 * MessageData.cpp
 *
 *  Created on: Nov 17, 2021
 *      Author: Family
 */
#include "MessageData.h"

using namespace std;

MessageData::MessageData(const char* buffer, size_t bufferLen)
{
	// The buffer has:
	//    <from-string>%<to-string>%<message-string>0
	// in it.  Since it ends in a zero, we can make a string out
	// of it, and extract the from, to and message strings.
	string bufferStr(buffer);
	size_t percent1Index = bufferStr.find('%', 0);
	size_t percent2Index = bufferStr.find('%', percent1Index + 1);
	m_from = bufferStr.substr(0, percent1Index);
	m_to = bufferStr.substr(percent1Index + 1, percent2Index - percent1Index - 1);
	m_message = bufferStr.substr(percent2Index + 1, string::npos);
}

// To get a single string encoding the message data, that
// can then be sent.
string MessageData::getSendString() const
{
	string dataStr = m_from + '%' + m_to + '%' + m_message;
	return dataStr;
}


const string& MessageData::getFrom() const
{
	return m_from;
}


const string& MessageData::getTo() const
{
	return m_to;
}


const string& MessageData::getMessage() const
{
	return m_message;
}






