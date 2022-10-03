/*
 * Sequence.h
 *
 *  Created on: Aug 5, 2021
 *      Author: R. Krishnaswamy
 *
 */

#include <limits>
#include <string>
#include <vector>
#include <exception>
#include <functional>

#pragma once

using namespace std;

// The Elements of a sequence are in zero-based indices.
typedef size_t IndexType;

// Forward declaration
class Rotation;


// The class Sequence is the base class of an efficient
// sequence data structure.  It is a container of Elements,
// each of which can be accessed by its index in the sequence.
// It is possible to insert and delete elements in the sequence
// and efficiently maintain their indices.

class Sequence
{
public:
	// The Elements of a sequence are in zero-based indices.
	// There is a unique undefined value for the index
	static IndexType UndefinedIndex;

	// The Sequence is a sequence of Elements.  Clients subclass the
	// Element class in order to hold them in a Sequence.
	class Element
	{
	public:
		// Constructor
		Element() {}

		// Virtual destructor
		virtual ~Element() {}

		// This is to get an image of this Element, usually
		// for debugging purposes.
		virtual string image() const;

		// To assign the value of another Element to this
		// one.  This assignment only changes attributes
		// of this element that are unrelated to its
		// position in the Sequence
		virtual
		Element& operator=(const Element& that)
		{
			m_parent = that.m_parent;
			m_left = that.m_left;
			m_right = that.m_right;
			m_height = that.m_height;
			m_weight = that.m_weight;
			m_cumWidth = that.m_cumWidth;
			return *this;
		}

		// The width of an Element can be queried.  The width
		// must be set through Sequence::setWidth().
		IndexType getWidth() const;
	private:
		Element* m_parent = nullptr;
		Element* m_left = nullptr;
		Element* m_right = nullptr;

		// Each node has the following attributes related to the
		// subtree T rooted by the node:
		//    - Height (longest distance to a leaf) in T
		//    - Weight (number of nodes) in T
		//    - Cumulative width of all the nodes in T

		// Maximum distance to a leaf node of subtree T
		IndexType m_height = 0;

		// Number of nodes in subtree T including this one itself.
		IndexType m_weight = 0;

		// Cumulative width of all the nodes in subtree T
		IndexType m_cumWidth = 0;

		friend class Sequence;
		friend class Rotation;
	};

	// Constructor
	Sequence();

	// Virtual destructor
	virtual ~Sequence();

	// Destroys all elements of the sequence, so it can be reused.
	void clear();

	// Current length of the sequence.  This is one more than the last
	// index.  Initial length of the sequence is zero.
	IndexType getLength();

	// To insert an element, and provide a width.  If pBeforeElt is
	// nullptr then pNewElt is inserted at the end (appended).  If
	// pNewElt is already in this or any other sequence, an exception
	// is thrown.  The last defaulted parameter provides the width
	// of the new element.
	void insert(Element* pNewElt, Element* pBeforeElt, IndexType width = 0);

	// To insert an element at a particular (zero-based) index, and
	// provide a width.  Valid indices are from zero to length().  If
	// the index is length(), then the new element is inserted at the
	// end (appended).  The last defaulted parameter provides the width
	// of the new element.
	void insertAtIndex(Element* pNewElt, IndexType atIndex, IndexType width = 0);

	// To append an element.  The last defaulted parameter provides the
	// width of the new element.
	void append(Element* pNewElt, IndexType width = 0);

	// To remove an element from the sequence.  The element is destroyed.
	void remove(Element* pElt);

	// To remove an element from the sequence.  The element is destroyed.
	void remove(IndexType index);

	// To replace an element at a particular (zero-based) index.  Valid
	// indices are from zero to length()-1.  Note that this method takes
	// an Element by reference, and its contents are copied into the
	// Element at this index.  The last defaulted parameter provides the
	// width of the new element.
	void replace(const Element& elt, IndexType atIndex, IndexType width = 0);

	// Each Element has a "width" attribute that indicates how much space
	// it occupies.  This is by default 0 if not specified.  It can be
	// specified by this method.
	void setWidth(Element* pElt, IndexType width);

	// The width of an Element can be queried.
	IndexType getWidth(const Element* pElt) const;

	// The start offset of an element can be queried.
	IndexType getStartOffset(const Element* pElt) const;

	// To get an element at a particular index
	Element* getElement(IndexType index) const;

	// Each element occupies an extant specified by its start offset and
	// its width.  This gets the element whose extent spans the given offset.
	Element* getElementAtOffset(IndexType offset) const;

	// To get the index of the element
	IndexType getIndex(Element* pElt) const;

	// To visit all the nodes in order.
	void visitInOrder
	        (std::function<void(const Element* pElt)> visitElt) const;

	// For printing the sequence
	void print() const;

	// For printing the sequence as a tree.
	void printTree() const;

	// To verify tree properties.  The height and weight of nodes need
	// to be correct.  Furthermore, the tree should be balanced, i.e.
	// that every node has left and right subtrees with heights differing
	// by at most one.  It throws an exception on first node that either
	// has incorrect height, weight or is unbalanced.  This method is
	// used in testing.
	void verify() const;

	// This is used for testing.
	Element* getRoot();

	// Gets the height-delta between the left and right subtrees.
	static
	int heightDelta(Sequence::Element* pElt);

	// Prints out which rotations were matched, and which were not.
	static void printRotationUsage();

private:
	Element* m_root;

	// Rebalance at pElt or some ancestor of it which is unbalanced.
	// Out value is the newly rebalanced root.
	void rebalance(Element*& pElt);

	void destroySubtree(Element* pElt);

	void visitInOrder
	        (const Element* pElt,
			 std::function<void(const Element* pElt)> visitElt) const;

	void printTree(const Element* pElt, IndexType depth) const;

	// Updates m_height, m_weight and m_cumWidth attributes.
	void updateAttributes(Element* pElt, IndexType width);

	void verify(Element* pElt) const;

	void assignInPlace(const Element* pSrcElt, Element* pDstElt);
};


