/*
 * Test.cpp
 *
 *  Created on: Aug 11, 2021
 *      Author: Family
 */

#include "inc/Sequence.h"
#include "inc/GenericSequence.h"
#include "TestUtilities.h"

#include <iostream>
#include <exception>

void testBasic(size_t count)
{
	Sequence seq;
	size_t countOdd;
	size_t countEven;
	TestElement* pElt;

	std::cout << "Started testBasic: " << count << std::endl;

	// The number of even indices in the sequence is countEven,
	// and the number of odd indices is countOdd.  A sequence
	// of 4 elements has indices 0,1,2,3 and a sequence of 5
	// elements has indices 0,1,2,3,4.  In both cases, the
	// odd-indexed elements are at indices 1 and 3.  So
	// countOdd = count/2.
	countOdd = count/2;
	if ((count%2) == 0) {
		// For example, count=4 has a sequence with indices
		// 0,1,2,3.  There are 2 odd and 2 even indices.
		countEven = countOdd;
	} else {
		// For example, count=5 has a sequence with indices
		// 0,1,2,3,4.  There are 2 odd and 3 even indices.
		countEven = countOdd + 1;
	}

	// Add even elements 0,2,4,6,8,10, count/2
	for (size_t i = 0; i < countEven; i++) {
		pElt = new TestElement(2*i);
		seq.append(pElt);

		// Set the width of each element.  The element width
		// is one more than its value.
		seq.setWidth(pElt, 2*i + 1);
	}

//	seq.printTree();
	std::cout << "Even only is: " << std::endl;
	seq.print();
	seq.verify();

	for (size_t i = 0; i < countEven; i++) {
		pElt = (TestElement*)seq.getElement(i);
		if (pElt->getValue() != 2*i) {
			string msg("Invalid evens sequence at index: ");
			msg += std::to_string(i);
			throw std::logic_error(msg);
		}
	}

	// Add odd elements
	// If count=11, the sequence at this point is 0,2,4,6,8,10
	// So we will add:
	//   9 before 10, at index 5
	//   7 before 8, at index 4
	//   5 before 6, at index 3
	// and so on.  Notice that we are adding elements from
	// the largest index (5) downwards.  This way, the earlier
	// indices do not change.  If we added elements from
	// 0 to 5, then after adding the first, the subsequent
	// indices would have to compensate for the added
	// elements.  This problem is avoided by going 0 to 5.
	size_t j;
	for (size_t i = 1; i <= countOdd; i++) {
		// The index j counts backwards from countOdd to 1
		j = countOdd + 1 - i;
		pElt = new TestElement(2*j - 1);
		seq.insertAtIndex(pElt, j);

		// Set the width of each element. The element width
		// is one more than its value.
		seq.setWidth(pElt, 2*j);
	}

//	seq.printTree();
	std::cout << "Complete sequence is: " << std::endl;
	seq.print();
	seq.verify();

	for (size_t i = 0; i < count; i++) {
		pElt = (TestElement*)seq.getElement(i);
		if (pElt->getValue() != i) {
			string msg("Invalid complete sequence at index: ");
			msg += std::to_string(i);
			throw std::logic_error(msg);
		}
	}

	// Now remove elements at odd indices.  Again do it backwards.
	for (size_t i = 1; i <= countOdd; i++) {
		// The index j counts backwards from countOdd to 1
		j = countOdd + 1 - i;
		seq.remove(2*j - 1);
	}

//	seq.printTree();
	std::cout << "Even only elements is: " << std::endl;
	seq.print();
	seq.verify();

	for (size_t i = 0; i < countEven; i++) {
		pElt = (TestElement*)seq.getElement(i);
		if (pElt->getValue() != 2*i) {
			string msg("Invalid sequence after removal of odd indices at index: ");
			msg += std::to_string(i);
			throw std::logic_error(msg);
		}
	}

	// Empty sequence, and refill it, this time from the beginning.
	seq.clear();
	for (size_t i = 0; i < count; i++) {
		pElt = new TestElement(count - 1 - i);
		seq.insertAtIndex(pElt, 0);
	}

	// Replace all elements at multiples of 10 by another which is
	// previous element multiplied by 10.
	TestElement testElement;
	for (size_t i = 0; i < count; i += 10)
	{
		testElement.setValue(10*i);
		seq.replace(testElement, i);
	}

	std::cout << "Original sequence after replacing every 10'th element is: " << std::endl;
	seq.print();
	seq.verify();

	// Now destroy the tree from the root.
	Sequence::Element* pRoot = seq.getRoot();
	while (pRoot != nullptr) {
// This is for debugging the failing case of an unbalanced tree
// after removing the root.  The call to testBasic(10) failed.
// We needed to put in LR-d and RL-d to take care of this and
// related cases.
//		std::cout << "Removing: " << pRoot->image() << std::endl;
//		pElt = (TestElement*) pRoot;
//		if (pElt->getValue() == 13) {
//			std::cout << "Tree before removal of 13" << std::endl;
//			seq.printTree();
//		}

		// When testBasic(10) is called, this results in an unbalanced
		// tree.  It appears to be a missing rotation in the set of
		// AVL rotations.  We needed to add LR-d and RL-d for this.
		seq.remove(pRoot);

		// After adding LR-d and RL-d, this seq.verify() passes for
		// all cases.
		seq.verify();
		pRoot = seq.getRoot();
	}

	std::cout << "Completed testBasic" << std::endl << std::endl;
}


void testBasicGeneric(size_t count)
{
	class Element1 {
	public:
		Element1()
		{}

		virtual ~Element1() {}

		void setValue(size_t data)
		{
			m_value = data;
		}

		size_t getValue()
		{
			return m_value;
		}

		virtual string image() const
		{
			return std::to_string(m_value);
		}

		virtual
		Element1& operator=(const Element1& that)
		{
			m_value = that.m_value;
			return *this;
		}

		bool operator !=(const Element1& that)
		{
			return m_value != that.m_value;
		}
	private:
		size_t m_value = 0;
	};

	std::cout << std::endl << "Started testBasicGeneric: " << count << std::endl;

	GenericSequence<Element1> seq;

	Element1 elt;
	for (size_t i = 0; i < count; i++) {
		elt.setValue(i);
		seq.append(elt, i+1);
	}

	seq.printTree();

	// Replace every 10'th element.
	for (size_t i = 0; i < count; i += 10)
	{
		seq[i].setValue(10*i);
	}

	// Print the sequence
	seq.print();
	seq.verify();

	// Reset every 10'th element
	for (size_t i = 0; i < count; i += 10)
	{
		seq[i].setValue(i);
	}

	// Check the sequence
	IndexType width;
	IndexType offset;
	Element1 elt2;
	IndexType startOffset = 0;
	for (size_t i = 0; i < count; i++) {
		elt = seq[i];
		if (i != elt.getValue()) {
			string msg = "Unexpected element at index " +
					     std::to_string(i) + ": " + elt.image();
			throw logic_error(msg);
		}

		if (startOffset != seq.getStartOffset(i)) {
			string msg = "Unexpected start offset at index " +
					     std::to_string(i) + ": " + elt.image();
			throw logic_error(msg);
		}

		width = seq.getWidth(i);
		if ((i+1) != width) {
			string msg = "Unexpected width of element at index " +
						 std::to_string(i) + ": " + elt.image();
			throw logic_error(msg);
		}

		offset = startOffset + width - 1;
		elt2 = seq.getElementAtOffset(offset);
		if (elt != elt2) {
			string msg = "Unexpected element at offset " +
						 std::to_string(offset) + ": " + elt2.image();
			throw logic_error(msg);
		}

		startOffset += width;
	}

	std::cout << "Completed testBasicGeneric" << std::endl << std::endl;
}


void checkSequenceforTestRandom(Sequence& seq)
{
	seq.verify();

	Sequence::Element* pElt;
	Sequence::Element* pElt2;
	TestElement* pTestElt;
	IndexType width;
	IndexType offset;
	IndexType expectedOffset = 0;
	IndexType count = seq.getLength();
	for (IndexType i = 0; i < count; i++) {
		pElt = seq.getElement(i);
		pTestElt = (TestElement*) pElt;

		if (pTestElt->getValue() != i) {
			string msg = "Unexpected element at index " +
					     std::to_string(i) + ": " + pElt->image();
			throw logic_error(msg);
		}

		offset = seq.getStartOffset(pElt);
		if (expectedOffset != offset) {
			string msg = "Unexpected offset for element at index " +
					     std::to_string(i) + ": " + std::to_string(offset);
			throw logic_error(msg);
		}

		width = pElt->getWidth();
		if (width != (i + 1)) {
			string msg = "Unexpected width for element at index " +
					     std::to_string(i) + ": " + std::to_string(width);
			throw logic_error(msg);
		}

		pElt2 = seq.getElementAtOffset(offset + width - 1);
		if (pElt2 != pElt) {
			string img = (pElt2 != nullptr)? pElt2->image() : "null";

			string msg = "Unexpected element at offset " +
					     std::to_string(i) + ": " + img;
			throw logic_error(msg);
		}

		// Change expectedOffset for the next element
		expectedOffset += width;
	}

}

// In parameter count is the size of the sequence that is edited.  Initially
// the sequence is constructed, then it is destructed by a sequence of
// deletes.  Then it is restored by a sequence of inserts in the opposite
// order.
void testRandom(size_t count)
{
	Sequence seq;
	TestElement* pElt;

	std::cout << std::endl << "Started testRandom: "
			  << count << std::endl;

	// Build the sequence
	for (size_t i = 0; i < count; i++) {
		pElt = new TestElement(i);
		seq.append(pElt);

		// Set the width of each element. The element width
		// is one more than its value.
		seq.setWidth(pElt, i + 1);
	}

	std::cout << "Initial sequence:" << std::endl;
	seq.print();

	checkSequenceforTestRandom(seq);

	// Now build a random delete sequence, deleting 'upto' elements.
	EditOpVec editOps;
	size_t currCount = count;
	size_t upto = count;
	size_t index;
	for (size_t i = 0; i < upto; i++)
	{
		index = rand() % currCount;

		// The edit sequence is a delete.  Thus we are deleting random
		// elements in the sequence.
		editOps.push_back(EditOp(index, false));

		// Decrement currCount so that the next index is
		// relative to the sequence with the above index
		// deleted.
		currCount--;
	}

	// Now do the editOps on the sequence.
	editOps.execDo(seq);

	// Now undo the editOps on the sequence.
	editOps.execUndo(seq);

	// Now print the sequence;
	std::cout << "Final sequence:" << std::endl;
	seq.print();

	checkSequenceforTestRandom(seq);

	std::cout << "Completed testRandom" << std::endl << std::endl;
}


void testEdits(const string& edits)
{
	Sequence seq;
	TestElement* pElt;

	std::cout << "Started testEdits: " << edits << std::endl;

	EditOpVec editOps(edits);

	size_t count = editOps.size();

	// Build the sequence
	for (size_t i = 0; i < count; i++) {
		pElt = new TestElement(i);
		seq.append(pElt);

		// Set the width of each element. The element width
		// is one more than its value.
		seq.setWidth(pElt, i + 1);
	}

	std::cout << "Initial sequence:" << std::endl;
	seq.print();

	// Now do the editOps on the sequence.
	editOps.execDo(seq);

	// Now undo the editOps on the sequence.
	editOps.execUndo(seq);

	// Now print the sequence;
	std::cout << "Final sequence:" << std::endl;
	seq.print();

	// Check the sequence
	for (size_t i = 0; i < count; i++) {
		pElt = (TestElement*) seq.getElement(i);
		if (i != pElt->getValue()) {
			string msg = "Unexpected element at index " +
					     std::to_string(i) + ": " + pElt->image();
			throw logic_error(msg);
		}

		if ((i + 1) != pElt->getWidth()) {
			string msg = "Unexpected width at index " +
					     std::to_string(i) + ": " + pElt->image();
			std::cout << "Edit Ops = " << editOps.image() << std::endl;
			throw logic_error(msg);
		}
	}

	std::cout << "Completed testEdits" << std::endl << std::endl;
}

// Build a sequence which matches the pre-modification shape for
// LRb and LRc
void buildShapeForLRbAndLRc(Sequence& seq)
{
	// Fill in a sequence of 15 elements, 0..14
	// This has the shape
	//   ----0
	//   --------3
	//   ------------1
	//   ----------------0
	//   ----------------2
	//   ------------5
	//   ----------------4
	//   ----------------6
	//   --------11
	//   ------------9
	//   ----------------8
	//   ----------------10
	//   ------------13
	//   ----------------12
	//   ----------------14
	TestElement* pElt;
	for (size_t i = 0; i < 15; i++) {
		pElt = new TestElement(i);
		seq.append(pElt);
	}

	// Delete elements 8, 10, 12 and 14 in reverse order,
	// so that indices are ok.
	// Now it has the shape
	//   ----0
	//   --------3
	//   ------------1
	//   ----------------0
	//   ----------------2
	//   ------------5
	//   ----------------4
	//   ----------------6
	//   --------11
	//   ------------9
	//   ------------13
	seq.remove(14);
	seq.remove(12);
	seq.remove(10);
	seq.remove(8);

	// It is now in the LR-b pre-modification shape (same as LR-c shape).
	std::cout << "Sequence tree for LR-b/c" << std::endl;
	seq.printTree();
}


void testLRb()
{
	Sequence seq;
	TestElement* pElt;

	std::cout << "Started testLRb" << std::endl;

	buildShapeForLRbAndLRc(seq);

	// Now add an element at (i.e. before) index 4.
	// This should invoke LR-b rotation.
	pElt = new TestElement(1000);
	seq.insertAtIndex(pElt, 4);

	seq.verify();
	std::cout << "Completed testLRb" << std::endl << std::endl;
}


void testLRc()
{
	Sequence seq;
	TestElement* pElt;

	std::cout << "Started testLRc" << std::endl;

	buildShapeForLRbAndLRc(seq);

	// Now add an element at (i.e. before) index 6.
	// This should invoke LR-c rotation.
	pElt = new TestElement(1000);
	seq.insertAtIndex(pElt, 6);

	seq.verify();

	std::cout << "Completed testLRc" << std::endl << std::endl;
}


int main()
{
	// The minimum sequence length for an unbalanced tree
	// is 3 (e.g. $0($1($2,0),0)).  Doing the above tests
	// from 3 to 40 covers all Rotations except for LR-b
	// and LR-c.
	for (size_t count = 3; count <= 40; count++) {
		testBasic(count);
		testBasicGeneric(count);
		testRandom(count);
	}

	testEdits("(2,D,2,3)(1,D,1,2)(1,D,3,8)(0,D,0,1)(0,D,4,5)");

	testLRb();
	testLRc();

	Sequence::printRotationUsage();
	return 0;
}
