/*
 * Rotation.cpp
 *
 *  Created on: Aug 6, 2021
 *      Author: Family
 */
#include <string>
#include <iostream>
#include "inc/Rotation.h"
#include <cctype>
#include <algorithm>

// Description of AVL trees
//   http://www.uvm.edu/~cbcafier/assets/cs124/19_AVL_Trees/AVL_Trees.pdf
// An animation of AVL trees
//   https://www.cs.usfca.edu/~galles/visualization/AVLtree.html


using namespace std;

// The value IGNORE_DELTA will be an ignored value for matching.
// It is used for leaf-nodes in a TreePattern.
const int Rotation::IGNORE_DELTA = std::numeric_limits<int>::max();

static
void getNextToken(const string& pattern,
		          string::size_type& from,
				  string& token)
{
	string::size_type upto = pattern.length();

	if (from >= upto) {
		token.clear();
		return;
	}

	// Skip past spaces.
	string::size_type i = from;
	while ((i < upto) && pattern[i] == ' ')
		i++;

	if (i == upto) {
		// Could not find $
		from = pattern.length();
		token.clear();
		return;
	}

	// First character of pattern
	switch (pattern[i])
	{
	case ',':
	case '(':
	case ')':
	case '0':
		// Return this one-character token
		token = pattern[i];
		from = i + 1;
		return;

	case '$':
		// Skip past the $
		i++;
		break;

	default:
		// Should not get here.
		break;
	}

	// The number must be immediately after a $.
	string::size_type numStart = i;
	while ((i < upto) && std::isdigit(pattern[i]))
		i++;
	if (i == numStart) {
		// Could not find digits.  Badly formed string.
		from = pattern.length();
		token.clear();
		return;
	}

	// The number is from numStart to i-1.  The token is $nnn
	// from numStart-1 to a length of i-(numStart-1).
	token = pattern.substr(numStart - 1, i - numStart + 1);

	// New value of from
	from = i;
}


struct StackEntry
{
	IndexType index;
	bool leftChild;

	StackEntry()
	{
		index = Sequence::UndefinedIndex;
		leftChild = false;
	}
};


static void buildTreePattern(const string& pattern,
		                     TreePattern& treePattern)
{
	string token;
	string::size_type from = 0;

	// First get the max index
	IndexType maxIndex = Sequence::UndefinedIndex;
	while (true) {
		getNextToken(pattern, from, token);
		if (token.empty()) {
			break;
		}

		if (token[0] == '$') {
			string num = token.substr(1);
			IndexType index = std::stoi(num);

			if ((maxIndex == Sequence::UndefinedIndex) ||
				(maxIndex < index)) {
				maxIndex = index;
			}
		}
	} // end of loop to get maxIndex

	if (maxIndex == Sequence::UndefinedIndex) {
		// Ill-formed string
		treePattern.reset();
		return;
	}

	vector<StackEntry> stack;

	treePattern.resize(maxIndex + 1);

	// Now populate treePattern.
	string numstr;
	StackEntry stackEntry;
	IndexType index, lastIndex;
	from = 0;
	while (true) {
		getNextToken(pattern, from, token);
		if (token.empty()) {
			break;
		}

		switch (token[0]) {
		case '$':
			numstr = token.substr(1);
			index = std::stoi(numstr);

			// Initialize the treePattern for this index
			treePattern[index].left = Sequence::UndefinedIndex;
			treePattern[index].right = Sequence::UndefinedIndex;

			if (stack.empty()) {
				treePattern.root = index;
			} else {
				stackEntry = stack.back();
				stack.pop_back();

				if (stackEntry.leftChild) {
					treePattern[stackEntry.index].left = index;
				} else {
					treePattern[stackEntry.index].right = index;
				}
			}

			lastIndex = index;
			break;

		case '(':
			// Expecting left and right children.  Push them on the stack.
			stackEntry.index = lastIndex;
			stackEntry.leftChild = false;
			stack.push_back(stackEntry);
			stackEntry.leftChild = true;
			stack.push_back(stackEntry);
			break;

		case '0':
			// This is the UndefinedIndex-entry in the pattern.  It represents
			// the nullptr in Sequence::Element.
			if (!stack.empty()) {
				stackEntry = stack.back();
				stack.pop_back();

				if (stackEntry.leftChild) {
					treePattern[stackEntry.index].left = Sequence::UndefinedIndex;
				} else {
					treePattern[stackEntry.index].right = Sequence::UndefinedIndex;
				}
			}

			break;

		case ',':
		case ')':
			break;

		default:
			break;
		}
	} // end of loop to populate treePattern.

}


// Default constructor
TreePattern::TreePattern()
{
	// Nothing
}


// Assignment operator
TreePattern::TreePattern(const TreePattern& that)
: vector<TreePatternEntry>(that)
{
	root = that.root;
}


void TreePattern::reset()
{
	this->clear();
	root = Sequence::UndefinedIndex;
}


void TreePattern::internalPostOrder(std::function<void(IndexType)> visit,
		                            IndexType fromIndex)
{
	if (fromIndex >= size()) {
		// Just checking!
		return;
	}

	TreePatternEntry& current = at(fromIndex);
	if (current.left != Sequence::UndefinedIndex) {
		internalPostOrder(visit, current.left);
	}

	if (current.right != Sequence::UndefinedIndex) {
		internalPostOrder(visit, current.right);
	}

	visit(fromIndex);
}


void TreePattern::postOrder(std::function<void(IndexType)> visit)
{
	internalPostOrder(visit, root);
}


Rotation::Rotation()
{
	// Nothing.
}


// Copy constructor
Rotation::Rotation(const Rotation& that)
{
	this->name = that.name;
	this->deltas = that.deltas;
	this->inputPattern = that.inputPattern;
	this->outputPattern = that.outputPattern;
	this->heightFixupIndices = that.heightFixupIndices;
}


void Rotation::init(const string& rotName,
		            const string& inPattern,
	   		        const vector<int>& heightDeltas,
			        const string& outPattern)
{
	name = rotName;
	deltas = heightDeltas;
	buildTreePattern(inPattern, inputPattern);
	buildTreePattern(outPattern, outputPattern);

	outputPattern.postOrder(
		[this] (IndexType index)->void {
			// This will print out the nodes in post order
			// std::cout << index << std::endl;

			// Put the internal nodes only in heightFixupIndices.
			// Pattern leaf nodes will not have their height changed.
			TreePatternEntry& entry = outputPattern[index];
			if (!entry.isLeaf()) {
				heightFixupIndices.push_back(index);
			}
		});

	/********************************************************
	size_t count = inputPattern.size();
	std::cout << "Root: " << inputPattern.root << std::endl;
	for (size_t i = 0; i < count; i++) {
		std::cout << i << ": "
				  << inputPattern[i].left
				  << ", "
				  << inputPattern[i].right
				  << std::endl;
	}

	count = inputPattern.size();
	std::cout << "Root: " << outputPattern.root << std::endl;
	for (size_t i = 0; i < count; i++) {
		std::cout << i << ": "
				  << outputPattern[i].left
				  << ", "
				  << outputPattern[i].right
				  << std::endl;
	}
	********************************************************/
}


// Applies the rotation if the inputPattern matches, and the
// indicated unbalancedNode is unbalanced as specified.  If
// the input pattern matched and the rotation was applied,
// the method returns true.  The out-value of pElt is the
// new root if the rotation was applied, and unchanged
// otherwise.
bool Rotation::rotate(Sequence& seq, Sequence::Element*& pElt)
{
	// Original parent of the root of the pattern
	Sequence::Element* pOriginalParent = pElt->m_parent;
	bool wasLeftChild = (pOriginalParent == nullptr)
			                ? false
			                : (pOriginalParent->m_left == pElt);

	// The matched Elements of the pattern are stored in nodes.
	size_t count = inputPattern.size();
	vector<Sequence::Element*> nodes(count);
	vector<IndexType> widths(count);

	// First populate the nodes.  This assumes the inputPattern
	// nodes are assigned in depth-first order from 0.  Hence,
	// iteration over the pattern from 0 is in depth-first order.
	size_t nodesIndex;
	Sequence::Element* pRover;
	nodes[inputPattern.root] = pElt;
	widths[inputPattern.root] = seq.getWidth(pElt);

	for (size_t i = 0; i < count; i++) {
		// Precondition: nodes[i] has been computed.
		// This is because inputPattern (and, in fact, outputPattern)
		// are in preorder (depth-first order).  So this loop
		// is in fact a depth first traversal of inputPattern.
		// We prime this precondition by the initialization of
		// nodes[inputPattern.root] before the loop.  Note that
		// inputPattern.root must be 0.
		TreePatternEntry& entry = inputPattern[i];

		// Children of leaf nodes are outside the pattern, and are
		// not examined in the matching.
		if (entry.isLeaf()) {
			continue;
		}

		// If nodes[i] is a null pointer, the entry must be a leaf.
		// Because of the prior test, the entry cannot be a leaf,
		// and so the match fails.
		if (nodes[i] == nullptr) {
			return false;
		}

		nodesIndex = (IndexType) entry.left;
		pRover = nodes[i]->m_left;
		if (nodesIndex == Sequence::UndefinedIndex) {
			if (pRover == nullptr) {
				// Matches.  Continue checking
			} else {
				// Input pattern did not match
				return false;
			}
		} else if (pRover == nullptr) {
			// Input pattern matches with a null pointer
			nodes[entry.left] = nullptr;
			widths[entry.left] = 0;
		} else {
			nodes[entry.left] = pRover;
			widths[entry.left] = seq.getWidth(pRover);
		}

		nodesIndex = (IndexType) entry.right;
		pRover = nodes[i]->m_right;
		if (nodesIndex == Sequence::UndefinedIndex) {
			if (pRover == nullptr) {
				// Matches.  Continue checking
			} else {
				// Input pattern did not match
				return false;
			}
		} else if (pRover == nullptr) {
			// Input pattern matches with a null pointer
			nodes[entry.right] = nullptr;
			widths[entry.right] = 0;
		} else {
			nodes[entry.right] = pRover;
			widths[entry.right] = seq.getWidth(pRover);
		}
	}

	// At this point, inputPattern matched.  Now check if the height-
	// deltas are correct.
	for (size_t i = 0; i < count; i++) {
		// It is an internal node.
		if (deltas[i] == IGNORE_DELTA) {
			continue;
		}

		if (nodes[i] == nullptr) {
			return false;
		}

		if (Sequence::heightDelta(nodes[i]) != deltas[i]) {
			return false;
		}
	}

//	std::cout << "Matched Rotation " << name
//			  << " at node " << pElt->image()
//			  << std::endl;

	// Mark the matched flag
	isMatched = true;

	// At this time, rebalancing is needed, i.e. deltas[0] == 2
	// or -2.  At this time, all the nodes[i] have been computed,
	// through the depth-first traversal of inputPattern in the
	// earlier matching loop.
	IndexType lchild, rchild;
	for (size_t i = 0; i < count; i++) {
		pRover = nodes[i];

		// A node that is a leaf in the input pattern and also a
		// leaf in the output pattern is not rotated ... no children
		// of such a node should change.
		if (inputPattern[i].isLeaf() && outputPattern[i].isLeaf()) {
			continue;
		}

		// Now set the new children of pRover based on the outputPattern.
		lchild = outputPattern[i].left;
		rchild = outputPattern[i].right;
		if(lchild == Sequence::UndefinedIndex) {
			pRover->m_left = nullptr;
		} else {
			pRover->m_left = nodes[lchild];
			if (nodes[lchild] != nullptr) {
				nodes[lchild]->m_parent = pRover;
			}
		}

		if(rchild == Sequence::UndefinedIndex) {
			pRover->m_right = nullptr;
		} else {
			pRover->m_right = nodes[rchild];
			if (nodes[rchild] != nullptr) {
				nodes[rchild]->m_parent = pRover;
			}
		}

		// We will be fixing the heights of internal nodes of outputPattern
		// for internal nodes of the outputPattern.  However, if a node of
		// outputPattern is a leaf node, we will not fixup its height and
		// weight, as they will have the height and weight of subtrees that
		// are outside the pattern.
		//
		// However, if a previously internal node of inputPattern has newly
		// become a leaf node of outputPattern, then its height, weight
		// and width need to be set.
		if ((pRover->m_left == nullptr) && (pRover->m_right == nullptr)) {
			pRover->m_height = 0;
			pRover->m_weight = 1;
			pRover->m_cumWidth = widths[i];
		}
	}

	// Nodes have been rebalanced.  Now to fix up the heights.  We do
	// a bottom-up traversal of the output pattern to fix up heights.
	// We need to do a bottom-up traversal of only the internal (non-
	// leaf) outputPattern nodes.  This is exactly heightFixupIndices.
	// Note that the height-fixup indices are in *post-order*.  Hence,
	// the root index is the last.
	count = heightFixupIndices.size();
	for (size_t i = 0; i < count; i++) {
		pRover = nodes[heightFixupIndices[i]];
		pRover->m_cumWidth = widths[heightFixupIndices[i]];

		// As pRover is an internal node of the outputPattern, at least
		// one of pRover->m_left and pRover->m_right is non-null.  At
		// this time, pRover->m_cumWidth is just the width of the node
		// itself and does not include its children.
		if (pRover->m_left == nullptr) {
			pRover->m_height = pRover->m_right->m_height + 1;
			pRover->m_weight = pRover->m_right->m_weight + 1;
			pRover->m_cumWidth += pRover->m_right->m_cumWidth;
		} else if (pRover->m_right == nullptr) {
			pRover->m_height = pRover->m_left->m_height + 1;
			pRover->m_weight = pRover->m_left->m_weight + 1;
			pRover->m_cumWidth += pRover->m_left->m_cumWidth;
		} else {
			// Both non-null
			pRover->m_height = std::max(pRover->m_left->m_height,
					                      pRover->m_right->m_height) + 1;
			pRover->m_weight = pRover->m_left->m_weight +
					             pRover->m_right->m_weight + 1;
			pRover->m_cumWidth += pRover->m_left->m_cumWidth +
								   pRover->m_right->m_cumWidth;
		}
	}

	// Fixup the original parent.
	pRover = nodes[outputPattern.root];
	pRover->m_parent = pOriginalParent;
	if (pOriginalParent != nullptr) {
		if (wasLeftChild) {
			pOriginalParent->m_left = pRover;
		} else {
			pOriginalParent->m_right = pRover;
		}
	}

	// Now traverse towards root, changing heights only.  No change of weights
	// or widths is necessary since the we only did a rotation.
	pRover = pOriginalParent;
	while (pRover != nullptr) {
		if ((pRover->m_left != nullptr) && (pRover->m_right != nullptr)) {
			pRover->m_height = std::max(pRover->m_left->m_height,
										  pRover->m_right->m_height) + 1;
		} else if (pRover->m_left != nullptr) {
			pRover->m_height = pRover->m_left->m_height + 1;
		} else if (pRover->m_right != nullptr) {
			pRover->m_height = pRover->m_right->m_height + 1;
		}
		pRover = pRover->m_parent;
	}

	// Now change pElt.
	pElt = nodes[outputPattern.root];

	// Done
	return true;
}


// Whether this rotation was ever used.
bool Rotation::isUsed() const
{
	return isMatched;
}


// To get the name
string Rotation::getName() const
{
	return name;
}

