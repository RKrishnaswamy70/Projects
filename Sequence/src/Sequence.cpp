/*
 * Sequence.cpp
 *
 *  Created on: Aug 5, 2021
 *      Author: Family
 */
#include <cctype>
#include "inc/Sequence.h"
#include "inc/Rotation.h"
#include <sstream>
#include <iostream>
#include <algorithm>

// Initialization of UndefinedIndex
IndexType Sequence::UndefinedIndex = std::numeric_limits<IndexType>::max();

// The set of Rotations.
static
vector<Rotation> s_avlRotations;


// This is to get an image of this Element, usually
// for debugging purposes.
string Sequence::Element::image() const
{
	// Default implementation
	ostringstream strm;

	strm << "0x" << std::hex << this << std::dec;
	return strm.str();
}


// Constructor
Sequence::Sequence()
: m_root(nullptr)
{
	if (!s_avlRotations.empty()) {
		// The avl rotations array is already initialized
		return;
	}

	// Need to initialize the avl rotations array.

	// Note that we will give the height deltas that are to match
	// for internal nodes of the inputPattern.  Since the deltas
	// vector is for all nodes (including leaf-nodes), we will
	// simply give a bogus value.
	int X = Rotation::IGNORE_DELTA;

	// LL-a Rotation
	// Note that in pattern LL-a, non-leaf node $0 of inPattern becomes
	// a leaf-node in outPattern.
	s_avlRotations.push_back(Rotation());
	Rotation* pRot = &s_avlRotations.back();
	pRot->init("LL-a","$0($1($2,0),0)", {2,1,X}, "$1($2,$0))");

	// LL-b Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("LL-b", "$0($1($2,$3),$4)", {2,1,X,X,X}, "$1($2,$0($3,$4))");

	// RR-a Rotation
	// Note that in pattern RR-a, non-leaf node $0 of inPattern becomes
	// a leaf-node in outPattern.
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RR-a", "$0(0,$1(0,$2))", {-2,-1,X}, "$1($0,$2))");

	// RR-b Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RR-b", "$0($1,$2($3,$4))", {-2,X,-1,X,X}, "$2($0($1,$3),$4)");

	// LR-a Rotation
	// Note that in pattern LR-a, non-leaf node $0 of inPattern becomes
	// a leaf-node in outPattern.
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("LR-a", "$0($1(0,$2),0)", {2,-1,0}, "$2($1,$0)");

	// LR-b Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("LR-b", "$0($1($2,$3($4,$5)),$6)", {2,-1,X,1,X,X,X}, "$3($1($2,$4),$0($5,$6))");

	// LR-c Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("LR-c", "$0($1($2,$3($4,$5)),$6)", {2,-1,X,-1,X,X,X}, "$3($1($2,$4),$0($5,$6))");

	// LR-d Rotation (this was not in text by Horowitz & Sahni)
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("LR-d", "$0($1($2,$3),$4)", {2,0,X,X,0}, "$1($2,$0($3,$4)");

	// RL-a Rotation
	// Note that in pattern RL-a, non-leaf node $0 of inPattern becomes
	// a leaf-node in outPattern.
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RL-a", "$0(0,$1($2,0))", {-2,1,0}, "$2($0,$1)");

	// RL-b Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RL-b", "$0($1,$2($3($4,$5),$6))", {-2,X,1,1,X,X,X}, "$3($0($1,$4),$2($5,$6))");

	// RL-c Rotation
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RL-c", "$0($1,$2($3($4,$5),$6))", {-2,X,1,-1,X,X,X}, "$3($0($1,$4),$2($5,$6))");

	// RL-d Rotation (this was not in text by Horowitz & Sahni)
	s_avlRotations.push_back(Rotation());
	pRot = &s_avlRotations.back();
	pRot->init("RL-d", "$0($1,$2($3,$4))", {-2,0,0,X,X}, "$2($0($1,$3),$4)");
}


// Virtual destructor
Sequence::~Sequence()
{
	clear();
}


void Sequence::clear()
{
	if (m_root != nullptr) {
		destroySubtree(m_root);
	}

	m_root = nullptr;
}


void Sequence::destroySubtree(Element* pElt)
{
	if (pElt->m_left != nullptr) {
		destroySubtree(pElt->m_left);
	}

	if (pElt->m_right != nullptr) {
		destroySubtree(pElt->m_right);
	}

	delete pElt;
}


// Current length of the sequence.  This is one more than the last
// index.  Initial length of the sequence is zero.
IndexType Sequence::getLength()
{
	if (m_root == nullptr) {
		return 0;
	}

	return m_root->m_weight;
}


// To insert an element, and provide a width.  If pBeforeElt is
// nullptr then pNewElt is inserted at the end (appended).  If
// pNewElt is already in this or any other sequence, an exception
// is thrown.  The last defaulted parameter provides the width
// of the new element.
void Sequence::insert(Element* pNewElt, Element* pBeforeElt, IndexType width)
{
	Element* pRover = nullptr;

	// If pBeforeElt is null, then pNewElt is appended to the sequence.
	if (pBeforeElt == nullptr) {
		if (m_root == nullptr) {
			// The very first element!

			// Fixup pNewElt
			pNewElt->m_parent = nullptr;
			m_root = pNewElt;
		} else {
			// Search for the last element.
			pRover = m_root;
			while (pRover->m_right != nullptr) {
				pRover = pRover->m_right;
			}

			// Now append it to this last element.
			pNewElt->m_parent = pRover;
			pRover->m_right = pNewElt;
		}

		updateAttributes(pNewElt, width);
		rebalance(pNewElt);

		// Done with appending.
		return;
	}

	// It is a true insertion.
	if (pBeforeElt->m_left == nullptr) {
		pNewElt->m_parent = pBeforeElt;
		pBeforeElt->m_left = pNewElt;
	} else {
		// Need to go to the rightmost descendant of
		// the left subtree of pBeforeElt.
		pRover = pBeforeElt->m_left;
		while (pRover->m_right != nullptr) {
			pRover = pRover->m_right;
		}

		// Now add it as a right descendant of pRover
		pNewElt->m_parent = pRover;
		pRover->m_right = pNewElt;
	}

	updateAttributes(pNewElt, width);
	rebalance(pNewElt);
}


// To insert an element at a particular (zero-based) index, and
// provide a width.  Valid indices are from zero to length().  If
// the index is length(), then the new element is inserted at the
// end (appended).  The last defaulted parameter provides the width
// of the new element.
void Sequence::insertAtIndex(Element* pNewElt, IndexType atIndex, IndexType width)
{
	IndexType length = getLength();

	if (atIndex > length) {
		// Error
		throw std::length_error("Invalid index!");
	}

	if (length == 0) {
		insert(pNewElt, nullptr, width);
	} else {
		Element* pBeforeElt = getElement(atIndex);
		insert(pNewElt, pBeforeElt, width);
	}
}


// To append an element.  The last defaulted parameter provides the
// width of the new element.
void Sequence::append(Element* pNewElt, IndexType width)
{
	insert(pNewElt, nullptr, width);
}


void Sequence::assignInPlace(const Element* pSrcElt, Element* pDstElt)
{
	// Get the width of the source element
	IndexType srcWidth = pSrcElt->getWidth();

	Element* pParent = pDstElt->m_parent;
	Element* pLeft = pDstElt->m_left;
	Element* pRight = pDstElt->m_right;
	IndexType height = pDstElt->m_height;
	IndexType weight = pDstElt->m_weight;
	IndexType cumWidth = pDstElt->m_cumWidth;

	// Copy elt into pElt.
	*pDstElt = *pSrcElt;

	// Restore sequence data members
	pDstElt->m_parent = pParent;
	pDstElt->m_left = pLeft;
	pDstElt->m_right = pRight;
	pDstElt->m_height = height;
	pDstElt->m_weight = weight;
	pDstElt->m_cumWidth = cumWidth;

	// Now set the width of pDstElt
	setWidth(pDstElt, srcWidth);
}


// To remove an element from the sequence.
void Sequence::remove(Element* pElt)
{
	Element* pRover;

	if (pElt == nullptr) {
		return;
	}

	if (pElt->m_right != nullptr) {
		// Has right child.  Find the successor, the
		// leftmost node of right child.  This includes
		// the case of having both children.
		pRover = pElt->m_right;
		while (pRover->m_left != nullptr) {
			pRover = pRover->m_left;
		}

		// At this point pRover is the successor of pElt.
		// Copy it into *pElt.
		assignInPlace(pRover, pElt);

		// Note: no need to update heights and weights
		// or rebalance as no nodes have been removed yet!

		remove(pRover);
	} else if (pElt->m_left != nullptr) {
		// Has left child.  Since pElt->m_right==nullptr, this
		// case has only the left child.  Find the predecessor,
		// the rightmost node of left child.
		pRover = pElt->m_left;
		while (pRover->m_right != nullptr) {
			pRover = pRover->m_right;
		}

		// At this point pRover is the predecessor of pElt.
		// Copy it into *pElt.
		assignInPlace(pRover, pElt);

		// Note: no need to update heights and weights
		// or rebalance as no nodes have been removed yet!

		remove(pRover);
	} else {
		// pElt is a leaf node with no child.
		IndexType parentWidth;
		Element* pParent = pElt->m_parent;
		if (pParent == nullptr) {
			m_root = nullptr;
		} else if (pParent->m_left == pElt) {
			// Save the parent width before changing its child
			parentWidth = pParent->getWidth();
			pParent->m_left = nullptr;
		} else {
			// Save the parent width before changing its child
			parentWidth = pParent->getWidth();
			pParent->m_right = nullptr;
		}

		delete pElt;

		if (pParent == nullptr) {
			return;
		}

		updateAttributes(pParent, parentWidth);

		// Now rebalance pParent, its parent, and so on all the
		// way to the root.
		while (pParent != nullptr) {
			rebalance(pParent);
			pParent = pParent->m_parent;
		}
	}
}


// To remove an element from the sequence.  The element is destroyed.
void Sequence::remove(IndexType index)
{
	Element* pElt = getElement(index);
	remove(pElt);
}


// To replace an element at a particular (zero-based) index.  Valid
// indices are from zero to length()-1.  Note that this method takes
// an Element by reference, and its contents are copied into the
// Element at this index.  The last defaulted parameter provides the
// width of the new element.
void Sequence::replace(const Element& elt, IndexType atIndex, IndexType width)
{
	IndexType length = getLength();

	if (atIndex >= length) {
		// Error
		throw std::length_error("Invalid index!");
	}

	Element* pElt = getElement(atIndex);

	assignInPlace(&elt, pElt);

	setWidth(pElt, width);
}


// To get an element at a particular index
Sequence::Element* Sequence::getElement(IndexType index) const
{
	Element* pElt = m_root;

	if ((pElt == nullptr) || index >= pElt->m_weight) {
		return nullptr;
	}

	IndexType leftWeight;
	IndexType indexInElt = index;
	while (pElt != nullptr) {
		if (pElt->m_left == nullptr) {
			if (indexInElt == 0) {
				return pElt;
			} else {
				// The element cannot be pElt.  It must
				// be in the right subtree of pElt.
				indexInElt--;
				pElt = pElt->m_right;
			}
		} else {
			leftWeight = pElt->m_left->m_weight;
			if (indexInElt < leftWeight) {
				// The element is in the left subtree of
				// pElt.  indexInElt is unchanged.
				pElt = pElt->m_left;
			} else if (indexInElt == leftWeight) {
				// pElt is the required element.
				return pElt;
			} else {
				// The required element is in the right subtree.
				indexInElt -= leftWeight + 1;
				pElt = pElt->m_right;
			}
		}
	}

	// Could not find the element.
	return nullptr;
}


// To get an element at a particular index
Sequence::Element* Sequence::getElementAtOffset(IndexType offset) const
{
	Element* pElt = m_root;

	if ((pElt == nullptr) || offset >= pElt->m_cumWidth) {
		return nullptr;
	}

	IndexType temp;
	IndexType width;
	IndexType leftWidth;
	IndexType offsetInElt = offset;
	while (pElt != nullptr) {
		width = pElt->getWidth();
		if (pElt->m_left == nullptr) {
			if (offsetInElt < width) {
				return pElt;
			} else {
				// The element cannot be pElt.  It must
				// be in the right subtree of pElt.
				offsetInElt -= width;
				pElt = pElt->m_right;

				// Control now re-iterates through the loop for
				// the right subtree.
			}
		} else {
			leftWidth = pElt->m_left->m_cumWidth;
			if (offsetInElt < leftWidth) {
				// The element is in the left subtree of
				// pElt.  indexInElt is unchanged.
				pElt = pElt->m_left;

				// Need to re-iterate through the loop for
				// the left subtree.
				continue;
			}

			temp = leftWidth + width;
			if (offsetInElt < temp) {
				// pElt is the required element.
				return pElt;
			} else {
				// The required element is in the right subtree.
				offsetInElt -= temp;
				pElt = pElt->m_right;

				// Control now re-iterates through the loop for
				// the right subtree.
			}
		}
	}

	// Could not find the element.
	return nullptr;
}


// Each Element has a "width" attribute that indicates how much space
// it occupies.  This is by default 1 if not specified.  It can be
// specified by this method.
void Sequence::setWidth(Element* pElt, IndexType width)
{
	bool isIncrease;
	IndexType delta;
	IndexType oldWidth = pElt->getWidth();

	if (oldWidth > width) {
		delta = oldWidth - width;
		isIncrease = false;
	} else if (oldWidth == width) {
		return;
	} else {
		delta = width - oldWidth;
		isIncrease = true;
	}

	Element* pRover = pElt;
	while (pRover != nullptr) {
		if (isIncrease) {
			pRover->m_cumWidth += delta;
		} else {
			if (pRover->m_cumWidth < delta) {
				string msg = "Node " + pElt->image() + " invalid width!";
				throw logic_error(msg);
			}
			pRover->m_cumWidth -= delta;
		}

		pRover = pRover->m_parent;
	}
}


// The width can also be queried.
IndexType Sequence::Element::getWidth() const
{
	IndexType width = m_cumWidth;

	if (m_left != nullptr) {
		width -= m_left->m_cumWidth;
	}

	if (m_right != nullptr) {
		width -= m_right->m_cumWidth;
	}

	return width;
}


// The width can also be queried.
IndexType Sequence::getWidth(const Element* pElt) const
{
	return pElt->getWidth();
}


// The start offset of an element can be queried.
IndexType Sequence::getStartOffset(const Element* pElt) const
{
	// The code is essentially the same as Sequence::getIndex
	const Element* pRover = pElt;

	IndexType startOffsetInRover;

	if (pRover->m_left == nullptr) {
		startOffsetInRover = 0;
	} else {
		startOffsetInRover = pRover->m_left->m_cumWidth;
	}

	const Element* pLeft;
	const Element* pParent = pRover->m_parent;

	while (pParent != nullptr) {
		pLeft = pParent->m_left;
		if (pLeft == pRover) {
			// startOffsetInRover is unchanged.
		} else if (pLeft == nullptr) {
			// startOffsetInRover has to be changed by the width of
			// pParent to account for the parent node.
			startOffsetInRover += pParent->getWidth();
		} else {
			// indexInCurrent has to be increased by the weight of
			// pLeft, plus width of the parent node.
			startOffsetInRover += pLeft->m_cumWidth + pParent->getWidth();
		}

		pRover = pParent;
		pParent = pRover->m_parent;
	}

	// Parent of pRover is nullptr, and so pRover must be the root.
	return startOffsetInRover;
}


// To get the index of the element
IndexType Sequence::getIndex(Element* pElt) const
{
	const Element* pRover = pElt;

	// indexInRover is the index of 'this' in the
	// subtree rooted by pRover.
	IndexType indexInRover;

	if (pRover->m_left == nullptr) {
		indexInRover = 0;
	} else {
		indexInRover = pRover->m_left->m_weight;
	}

	const Element* pLeft;
	const Element* pParent = pRover->m_parent;

	while (pParent != nullptr) {
		pLeft = pParent->m_left;
		if (pLeft == pRover) {
			// indexInRover is unchanged.
		} else if (pLeft == nullptr) {
			// indexInRover has to be changed by 1 to account
			// for the parent node.
			indexInRover++;
		} else {
			// indexInRover has to be increased by the weight of
			// pLeft, plus 1 for the parent node.
			indexInRover += pLeft->m_weight + 1;
		}

		pRover = pParent;
		pParent = pRover->m_parent;
	}

	// When pRover is the root, i.e. pParent == nullptr we are done.
	return indexInRover;
}


void Sequence::visitInOrder
             (std::function<void(const Element* pElt)> doVisit) const
{
	// Visit in order from the root at depth 0
	visitInOrder (m_root, doVisit);
}


void Sequence::print() const
{
	IndexType index = 0;
	visitInOrder([&index](const Element* pElt)->void {
		std::cout << index << ": "
				  << pElt->image()
				  << ", width = " << pElt->getWidth()
				  << std::endl;
		index++;
	});
}


void Sequence::visitInOrder
             (const Element* pElt,
		      std::function<void(const Element* pElt)> visitElt) const
{
	if (pElt->m_left != nullptr) {
		visitInOrder (pElt->m_left, visitElt);
	}

	visitElt(pElt);

	if (pElt->m_right != nullptr) {
		visitInOrder(pElt->m_right, visitElt);
	}
}


void Sequence::printTree(const Element* pElt, IndexType depth) const
{
	IndexType blanksCount = depth*4;
	for (IndexType i = 0; i < blanksCount; i++) {
		std::cout << '-';
	}

	if (pElt == nullptr) {
		std::cout << "null";
	} else {
		std::cout << pElt->image()
				  << " h:" << pElt->m_height
				  << " w:" << pElt->m_weight
				  << " width: " << pElt->getWidth();
	}
	std::cout << std::endl;

	if (pElt == nullptr) {
		return;
	}

	// If both subtrees are empty, don't bother printing them.
	// Thus leaf elements are printed without printing their
	// empty subtrees.
	if ((pElt->m_left == nullptr) && (pElt->m_right == nullptr)) {
		return;
	}

	printTree(pElt->m_left, depth + 1);
	printTree(pElt->m_right, depth + 1);
}


// For printing the sequence.
void Sequence::printTree() const
{
	printTree(m_root, 0);
}


// Updates m_height, m_weight and m_cumWidth attributes.
void Sequence::updateAttributes(Element* pElt, IndexType width)
{
	Element* pRover;
	Element* pLeft;
	Element* pRight;
	Element* pParent;
	IndexType newHeight;
	IndexType newWeight;
	IndexType newCumWidth;
	IndexType parentWidth = 0;

	pRover = pElt;
	newCumWidth = width;
	while (pRover != nullptr) {
		pLeft = pRover->m_left;
		pRight = pRover->m_right;
		newHeight = 0;
		newWeight = 1;

		if (pLeft != nullptr) {
			newHeight = pLeft->m_height + 1;
			newWeight += pLeft->m_weight;
			newCumWidth += pLeft->m_cumWidth;
		}

		if (pRight != nullptr) {
			newHeight = std::max(newHeight, pRight->m_height + 1);
			newWeight += pRight->m_weight;
			newCumWidth += pRight->m_cumWidth;
		}

		// Before we change the m_cumWidth of pRover, save the parent's
		// width.  This will be used to compute the m_cumWidth of the
		// parent in the next iteration.
		pParent = pRover->m_parent;
		if (pParent != nullptr) {
			parentWidth = pParent->getWidth();
		}

		// Now update attributes of pRover.
		pRover->m_height = newHeight;
		pRover->m_weight = newWeight;
		pRover->m_cumWidth = newCumWidth;

		// If we were only maintaining heights, we could quit when
		// pRover->m_height is unchanged.  However, with maintaining
		// the weight and width we need to go all the way to the root.
		// So update pRover to be pParent, and reinitialize newCumWidth
		// to be the width that was saved for pParent (parentWidth).
		pRover = pParent;
		newCumWidth = parentWidth;
	}
}


void Sequence::verify() const
{
	verify(m_root);
}


void Sequence::verify(Sequence::Element* pElt) const
{
	if (pElt == nullptr) {
		return;
	}

	// Check this node.
	long delta = 0;
	IndexType height = 0;
	IndexType weight = 1;
	if (pElt->m_left != nullptr) {
		delta = pElt->m_left->m_height;
		weight += pElt->m_left->m_weight;
		height = pElt->m_left->m_height + 1;
	}

	if (pElt->m_right != nullptr) {
		delta -= pElt->m_right->m_height;
		weight += pElt->m_right->m_weight;
		height = std::max(height, pElt->m_right->m_height + 1);
	}

	if ((delta < -1) || (delta > 1)) {
		printTree();
		// This node is unbalanced.
		string msg = "Node " + pElt->image() + " is unbalanced!";
		throw logic_error(msg);
	}

	// Confirm that the weight is correct.
	if (weight != pElt->m_weight) {
		// This node is unbalanced.
		string msg = "Node " + pElt->image() + " has incorrect weight!";
		throw logic_error(msg);
	}

	// Confirm that the height is correct.
	if (height != pElt->m_height) {
		// This node is unbalanced.
		string msg = "Node " + pElt->image() + " has incorrect height!";
		throw logic_error(msg);
	}

	// Now check child nodes.
	verify(pElt->m_left);
	verify(pElt->m_right);

	// It is balanced.
}


Sequence::Element* Sequence::getRoot()
{
	return m_root;
}


int Sequence::heightDelta(Sequence::Element* pElt)
{
	if ((pElt->m_left == nullptr) && (pElt->m_right == nullptr)) {
		return 0;
	} else if (pElt->m_left == nullptr) {
		// Negative of the right height + 1
		return -(int)(pElt->m_right->m_height + 1);
	} else if (pElt->m_right == nullptr) {
		// Left height + 1
		return (int) pElt->m_left->m_height + 1;
	} else {
		// Difference between left and right heights
		return (int) (pElt->m_left->m_height - pElt->m_right->m_height);
	}
}


void Sequence::rebalance(Element*& pElt)
{
	bool didRotate;

	// First find unbalanced ancestor.
	int delta;
	Element* pRover = pElt;
	while (pRover != nullptr) {
		delta = heightDelta(pRover);
		if ((delta == 2) || (delta == -2)) {
			break;
		}
		pRover = pRover->m_parent;
	}

	if (pRover == nullptr) {
		// Nothing was rebalanced.  So pElt is unchanged.
		return;
	}

	// Go through the vector of rotations, to see if any
	// match and rebalance
	size_t count = s_avlRotations.size();
	for (size_t i = 0; i < count; i++) {
		Rotation& rot = s_avlRotations[i];
		didRotate = rot.rotate(*this, pRover);
		if (didRotate) {
			if (pRover->m_parent == nullptr) {
				// Need to change the root.
				m_root = pRover;
			}

			// pElt is changed to the root of newly balanced subtree
			pElt = pRover;
			break;
		}
	}
}


// Prints out which rotations were matched, and which were not.
void Sequence::printRotationUsage()
{
	std::cout << std::endl << "Rotation usage:" << std::endl;
 	size_t count = s_avlRotations.size();
	for (size_t i = 0; i < count; i++) {
		Rotation& rot = s_avlRotations[i];

		if (rot.isUsed()) {
			std::cout << rot.getName() << ": used" << std::endl;
		} else {
			std::cout << rot.getName() << ": unused" << std::endl;
		}
	}
}



