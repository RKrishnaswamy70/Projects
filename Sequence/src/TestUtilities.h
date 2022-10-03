/*
 * TestUtilities.h
 *  Created on: Sep 13, 2021
 *      Author: Family
 */
#include "inc/Sequence.h"
#include <sstream>
#include <iostream>

#pragma once

class TestElement : public Sequence::Element
{
public:
	TestElement()
	{}

	TestElement(size_t value)
	: Sequence::Element(), m_value(value)
	{}

	// Virtual destructor
	virtual ~TestElement() {}

	virtual string image() const
	{
		return std::to_string(m_value);
	}

	size_t getValue()
	{
		return m_value;
	}

	void setValue(size_t value)
	{
		m_value = value;
	}

	virtual
	Sequence::Element& operator=(const Element& that)
	{
		TestElement* pThat = (TestElement*) &that;
		m_value = pThat->m_value;
		return Sequence::Element::operator =(that);
	}

private:
	size_t m_value = 0;
};


// An edit operation
class EditOp
{
public:
	EditOp(IndexType index, bool isInsert)
	{
		m_index = index;
		m_isInsert = isInsert;
	}

	EditOp(const EditOp& that)
	{
		m_index = that.m_index;
		m_isInsert = that.m_isInsert;
		m_value = that.m_value;
		m_width = that.m_width;
	}

	EditOp(const string& str)
	{
		size_t i = 0;
		size_t digStart = 0;

		i++; // skip past (

		// Pick up index
		digStart = i;
		while (isdigit(str[i])) i++;
		m_index = stoi(str.substr(digStart, i - digStart));
		i++; // skip past ,

		// Pick up type
		m_isInsert = (str[i] == 'I')? true : false;
		i++; i++; // skip past ,

		// Pick up value
		digStart = i;
		while (isdigit(str[i])) i++;
		m_value = stoi(str.substr(digStart, i - digStart));
		i++; // skip past ,

		digStart = i;
		while (isdigit(str[i])) i++;
		m_width = stoi(str.substr(digStart, i - digStart));
		i++; // skip past )
	}

	EditOp& operator =(const EditOp& that)
	{
		m_index = that.m_index;
		m_isInsert = that.m_isInsert;
		m_value = that.m_value;
		m_width = that.m_width;
		return *this;
	}

	void execDo(Sequence& seq)
	{
		if (m_isInsert) {
			TestElement* pNewTestElt = new TestElement(m_value);
			seq.insertAtIndex(pNewTestElt, m_index, m_width);
		} else {
			TestElement* pTestElt = (TestElement*) seq.getElement(m_index);
			m_value = pTestElt->getValue();
			m_width = pTestElt->getWidth();
			seq.remove(m_index);
		}
	}

	void execUndo(Sequence& seq)
	{
		// Temporarily invert the operation
		m_isInsert = !m_isInsert;

		// Do the inverse operation
		execDo(seq);

		// Undo the invertion, in case needed again.
		m_isInsert = !m_isInsert;
	}

	string image()
	{
		std::stringstream strm;
		char IorD = (m_isInsert)? 'I':'D';
		strm << "("
			 << m_index
			 << ","
			 << IorD
			 << ","
			 << m_value
			 << ","
			 << m_width
			 << ")";
		return strm.str();
	}

	IndexType m_index = 0;
	bool m_isInsert = false;

	// If this is a deletion, m_value set to the deleted value.
	// If this is an insertion, m_value is set to the inserted value.
	size_t m_value = 0;

	// If this is a deletion, m_width is set to the deleted width.
	// If this is an insertion, m_width is set to the inserted width.
	size_t m_width = 0;
};


class EditOpVec : public vector<EditOp>
{
public:
	EditOpVec()
	{}

	EditOpVec(const string& str)
	{
		size_t from;
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] == '(') {
				from = i;
			} else if (str[i] == ')') {
				push_back(EditOp(str.substr(from, i + 1 - from)));
			} else {
				// Nothing.
			}
		}
	}

	string image()
	{
		stringstream strm;
		size_t count = size();
		for (size_t i = 0; i < count; i++) {
			strm << at(i).image();
		}

		return strm.str();
	}

	void execDo(Sequence& seq)
	{
		size_t count = size();
		for (size_t i = 0; i < count; i++) {
			at(i).execDo(seq);
		}
	}

	void execUndo(Sequence& seq)
	{
		size_t count = size();
		if (count == 0) {
			return;
		}

		for (size_t i = 0; i < count; i++) {
			at(count - 1 - i).execUndo(seq);
			seq.verify();
		}
	}
};
