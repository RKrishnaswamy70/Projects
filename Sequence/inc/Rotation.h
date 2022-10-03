/*
 * Rotation.h
 *
 *  Created on: Aug 6, 2021
 *      Author: Family
 */

#pragma once

#include <functional>
#include "inc/Sequence.h"

// A Rotation specifies a rotation of the nodes of the tree that is
// done in order to rebalance it.  For instance, we may specify a
// rotation:
//      Input Pattern         Output Pattern
//           $1                    $2
//            |                     |
//        $0-----$2             $1-----$3
//                |
//            $0-----$3

// A TreePattern is small tree that represents a segment in a larger
// tree.  For instance, the Tree Pattern expression
//    $0($1($2 $3) $4($5 0))
// would be represented by the TreePattern
//      Node     Left-Child  Right-Child
//       0           1            4
//       1           2            3
//       2           0            0
//       3           0            0
//       4           5            0
//       5           0            0
//       6           0            0
// A TreePatternEntry is a struct giving the left and right child.
//
// Spaces and commas can be used with a TreePattern expression for
// readability.
struct TreePatternEntry
{
	IndexType left = Sequence::UndefinedIndex;
	IndexType right = Sequence::UndefinedIndex;

	bool isLeaf() {
		return (left == Sequence::UndefinedIndex) &&
			   (right == Sequence::UndefinedIndex);
	}
};


// A TreePattern is merely a vector of TreePatternEntry's, specifying
// for each node what its children are.
class TreePattern : public vector<TreePatternEntry>
{
public:
	// Specifies which index is the root.
	IndexType root = Sequence::UndefinedIndex;

	// Default constructor
	TreePattern();

	// Copy constructor
	TreePattern(const TreePattern& that);

	void reset();

	// Visits in postorder.  The parameter is called on each index
	// in postorder, the root being the last.
	void postOrder(std::function<void(IndexType)> visit);

private:
	void internalPostOrder(std::function<void(IndexType)> visit,
			               IndexType fromIndex);
};

// We can now describe a Rotation.
class Rotation
{
public:

	// Default constructor
	Rotation();

	// Copy constructor
	Rotation(const Rotation& that);

	// An input pattern matches if the tree has the
	// shape of inputPattern, and also if the for the node
	// that matches $n, the left-subtree-height minus the
	// right-subtree-height = deltas[n].  The value IGNORE_
	// DELTA will be an ignored value for matching.
	static const int IGNORE_DELTA;

	// Initializes a rotation.
	// Pattern strings are provided using a tree notation:
	// such as $0($1($2 $3) $4($5 0)) representing TreePattern
	//                $0
	//                 |
	//       $1------------------$4
	//        |                   |
	//   $2-------$3         $5-------nullptr
	// The above TreePattern will be translated by this constructor into
	// the TreePattern described earlier.  The heightDeltas vector specifies
	// that for the node that matches $i, the left-subtree-height minus
	// right-subtree-height should be deltas[i].  The value IGNORE_DELTA
	// for an entry of heightDeltas will be an ignored value for matching.
	//
	// Note that the nodes $0, $1, etc must be enumerated in pre-order.
	// that is to say:
	//   - $0 must be the first pre-order node (the root).
	//   - $1 must be the second pre-order node
	// and so on.
	//
	// Note also that leaf-nodes of inPattern must also be leaf-nodes of
	// outPattern.  However non-leaf nodes of an inPattern could become
	// a leaf-node of the outPattern (e.g. Sequence.cpp has pattern RR-a,
	// where non-leaf node $0 of inPattern becomes a leaf-node in outPattern).
	void init(const string& rotName,
			  const string& inPattern,
		      const vector<int>& heightDeltas,
		      const string& outPattern);

	// Applies the rotation if the inputPattern matches, and the
	// indicated unbalancedNode is unbalanced as specified.  If
	// the input pattern matched and the rotation was applied,
	// the method returns true.  The out-value of pElt is the
	// new root if the rotation was applied, and unchanged
	// otherwise.
	bool rotate(Sequence& seq, Sequence::Element*& pElt);

	// Whether this rotation was ever used.
	bool isUsed() const;

	// The name
	string getName() const;

private:
	// The name of the rotation
	string name = "";

	// Input and output patterns
	TreePattern inputPattern;
	TreePattern outputPattern;

	// These the height delta of the individual inputPattern
	// nodes.  An input pattern matches if the tree has the
	// shape of inputPattern, and also if the for the node
	// that matches $n, the left-subtree-height minus the
	// right-subtree-height = deltas[n].  The value IGNORE_
	// DELTA will be an ignored value for matching.
	vector<int> deltas;

	// Postorder traversal of output pattern.  These are the
	// indices that need to be fixed up in outputPattern
	// to compute new heights.
	vector<IndexType> heightFixupIndices;

	// Whether this rotation was ever matched.
	bool isMatched = false;
};


