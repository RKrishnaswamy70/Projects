/*
 * Sequence.h
 *
 *  Created on: Aug 5, 2021
 *      Author: R. Krishnaswamy
 *
 */

#include "inc/Sequence.h"

#pragma once

using namespace std;

// The template GenericSequence is an efficient sequence data structure.
// It is parameterized by the type Element, and is a container of Elements
// each of which can be accessed by its index in the sequence.  It enables
// insertion and deletion in the sequence and efficiently maintain the
// indices of the elements.

template <
// The class ElementType is expected to have:
//   - A virtual destructor
//   - An image method
//       virtual string image() const;
//   - An assignment operator
//       virtual ElementType& operator =(const ElementType& that);
class ElementType
>
class GenericSequence
{
public:
	// The Sequence is a sequence of GenericElement
	class GenericElement : public Sequence::Element
	{
	public:
		// Constructor
		GenericElement(const ElementType& data)
		{
			m_data = data;
		}

		// Virtual destructor
		virtual ~GenericElement() {}

		// This is to get an image of this Element, usually
		// for debugging purposes.
		virtual string image() const
		{
			return m_data.image();
		}

		// To assign the value of another Element to this
		// one.  This assignment only changes attributes
		// of this element that are unrelated to its
		// position in the Sequence
		virtual
		Element& operator=(const GenericElement& that)
		{
			m_data = that.m_data;
			return Sequence::Element::operator =(that);
		}

	private:
		ElementType m_data;
		friend GenericSequence;
	};

	// Constructor
	GenericSequence()
	{}

	// Virtual destructor
	virtual ~GenericSequence()
	{}

	// Destroys all elements of the sequence, so it can be reused.
	void clear()
	{
		m_seq.clear();
	}

	// Current length of the sequence.  This is one more than the last
	// index.  Initial length of the sequence is zero.
	IndexType getLength()
	{
		return m_seq.getLength();
	}

	// To insert an element at a particular (zero-based) index.  Valid
	// indices are from zero to length().  If the index is length(),
	// then the new element is inserted at the end (appended).  The last
	// defaulted parameter provides the width of the new element.
	void insertAtIndex(const ElementType& elt, IndexType atIndex, IndexType width = 0)
	{
		GenericElement* pGenElt = new GenericElement(elt);
		m_seq.insertAtIndex(pGenElt, atIndex, width);
	}

	// To append an element.  The last defaulted parameter provides the width
	// of the new element.
	void append(const ElementType& elt, IndexType width = 0)
	{
		GenericElement* pGenElt = new GenericElement(elt);
		m_seq.append(pGenElt, width);
	}

	// To remove an element from the sequence.  The element is destroyed.
	void remove(IndexType index)
	{
		m_seq.remove(index);
	}

	// This method will throw an exception unless index is between 0 and
	// count-1 where count is the number of elements in the GenericSequence.
	ElementType& operator[](IndexType index)
	{
		GenericElement* pGenElt = (GenericElement*) m_seq.getElement(index);
		if (pGenElt == nullptr) {
			throw std::range_error("Invalid index!");
		}

		return pGenElt->m_data;
	}

	// Each Element has a "width" attribute that indicates how much space
	// it occupies.  This is by default 0 if not specified.  It can be
	// specified by this method.
	void setWidth(IndexType index, IndexType width)
	{
		GenericElement* pGenElt = (GenericElement*) m_seq.getElement(index);
		if (pGenElt == nullptr) {
			throw std::range_error("Invalid index!");
		}

		m_seq.setWidth(pGenElt, width);
	}

	// The width of an Element can be queried.
	IndexType getWidth(IndexType index) const
	{
		GenericElement* pGenElt = (GenericElement*) m_seq.getElement(index);
		if (pGenElt == nullptr) {
			throw std::range_error("Invalid index!");
		}

		return m_seq.getWidth(pGenElt);
	}

	// The start offset of an element can be queried.
	IndexType getStartOffset(IndexType index) const
	{
		GenericElement* pGenElt = (GenericElement*) m_seq.getElement(index);
		if (pGenElt == nullptr) {
			throw std::range_error("Invalid index!");
		}

		return m_seq.getStartOffset(pGenElt);
	}

	// Each element occupies an extant specified by its start offset and
	// its width.  This gets the element whose extent spans the given offset.
	ElementType getElementAtOffset(IndexType offset) const
	{
		GenericElement* pGenElt = (GenericElement*) m_seq.getElementAtOffset(offset);
		if (pGenElt == nullptr) {
			throw std::range_error("Invalid index!");
		}

		return pGenElt->m_data;
	}

	// To visit all the nodes in order.
	void visitInOrder
	        (std::function<void(const ElementType& elt)> visitElt)
	{
		m_seq.visitInOrder([visitElt](GenericElement* pGenElt)->void {
			visitElt(pGenElt->m_data);
		});
	}

	// For printing the sequence
	void print()
	{
		m_seq.print();
	}

	// For printing the sequence as a tree.
	void printTree()
	{
		m_seq.printTree();
	}

	// To verify tree properties.  The height and weight of nodes need
	// to be correct.  Furthermore, the tree should be balanced, i.e.
	// that every node has left and right subtrees with heights differing
	// by at most one.  It throws an exception on first node that either
	// has incorrect height, weight or is unbalanced.  This method is
	// used in testing.
	void verify() const
	{
		m_seq.verify();
	}
private:
	Sequence m_seq;
};


