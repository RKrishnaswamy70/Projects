/*
 * Content.cpp
 *
 *  Created on: Feb 17, 2022
 *      Author: Family
 */

#include "Content.h"
#include <iostream>

Content::Content(const string& responseText)
{
	// The following extracts the contentStr, e.g:
	//   value1=aaa&value2=option1&value3=button1&value4=data4
	string contentLengthAttrName = "Content-Length: ";
	size_t contentLengthIndex =
			responseText.find(contentLengthAttrName);
	if (contentLengthIndex == string::npos) {
		// Invalid response text
		return;
	}
	contentLengthIndex += contentLengthAttrName.length();

	size_t contentLengthLen = 0;
	while (isdigit(responseText[contentLengthIndex + contentLengthLen]))
		contentLengthLen++;
	string contentLengthStr = responseText.substr(contentLengthIndex, contentLengthLen);
	size_t contentLength = std::stoi(contentLengthStr);

	// For a Post response method
	//    <form action="/response" method="post">
	// the content string is at the *end* of the responseText
	string contentStr = responseText.substr(responseText.length() - contentLength,
						 contentLength);

	// Now scan the string and fill in the map.
	string key;
	string value;
	size_t lastIndex = contentLength - 1;
	size_t keyIndex = 0;
	size_t keyLen = 0;
	size_t valueIndex = 0;
	size_t valueLen = 0;
	bool scanningKey = true;
	for (size_t i = 0; i < contentLength; i++) {
		if (scanningKey) {
			if (contentStr[i] == '=') {
				scanningKey = false;
				keyLen = i - keyIndex;
				key = contentStr.substr(keyIndex, keyLen);

				// Update valueIndex for next iteration
				valueIndex = keyIndex + keyLen + 1;
			}
		} else {
			if ((i == (lastIndex)) || (contentStr[i] == '&')) {
				scanningKey = true;
				if (i == lastIndex) {
					valueLen = i - valueIndex + 1;
				} else {
					valueLen = i - valueIndex;
				}

				value = contentStr.substr(valueIndex, valueLen);

				// Update keyIndex for next iteration
				keyIndex = valueIndex + valueLen + 1;

				// Enter into map
				contentMap[key] = value;
			}
		}
	}
}


string Content::getValue(const string& key)
{
	map<string, string>::iterator iter = contentMap.find(key);

	if (iter == contentMap.end()) {
		return "?";
	} else {
		return iter->second;
	}
}
