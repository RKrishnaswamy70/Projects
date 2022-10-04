#include <iostream>

#include <set>
#include <vector>
#include <list>

#include "Prufer.h"


static
void printSeqAndTree(const VertexSequence& vSeq,
                     const Graph& tree)
{
	// Go through the tree edges
	for (auto edge : tree)
	{
		std::cout << " (" << edge.first << "," << edge.second << ")";
	}

	std::cout << std::endl;
	for (auto v : vSeq)
	{
		std::cout << " " << v;
	}

	std::cout << std::endl;
	std::cout << std::endl;
}


// A test that can be called with a sequence of positive ints.
// It will convert it to a tree, and then back to the
// sequence, and validate that the two are the same. 
static
bool check(const VertexSequence& vSeq)
{
	Prufer prufer1(vSeq);
	const Graph& tree = prufer1.getTree();

	Prufer prufer2(tree);
	const VertexSequence vSeq2 = prufer2.getSequence();

	bool ok = (vSeq2 == vSeq);

	if (!ok)
	{
		// Since it is not ok, print it.
		printSeqAndTree(vSeq, tree);
	}

	return ok;
}

// This method generates the next element of the sequence,
// and returns false if there is none.  For instance if n=5
// it operates as follows
//     current vSeq         next vSeq
//       1,1,1               2,1,1
//       2,1,1               3,1,1
//       3,1,1               1,2,1
//       ...
//       3,2,2               3,3,2
//       3,3,2               3,3,3
//       3,3,3               none
static
bool getNext(VertexSequence& vSeq)
{
	size_t n = vSeq.size() + 2;
	VertexSequence::iterator firstIt;
	VertexSequence::iterator endIt = vSeq.end();

	// Find the first entry that can be incremented.  This
	// is the first entry that is not n.
	firstIt = vSeq.begin();
	while ((firstIt != endIt) && (*firstIt == n))
	{
		// This element can no longer be incremented.
		// Reset it to 1 (the lowest vertex) and continue
		*firstIt = 1;
		firstIt++;
	}

	// At this point, firstIt points to the first incrementable
	// vertex.  If it is at the end, we are done.
	if (firstIt == endIt)
	{
		return false;
	}

	// Increment the sequence at firstIt
	(*firstIt)++;

	// We have updated it.
	return true;
}


int main(void)
{
	VertexSequence vSeq = { 2, 2, 2, 3, 3, 4 };

	vSeq = { 2, 2, 2, 3, 3, 4 };
	check(vSeq);
	std::cout << "Passed tree 1." << std::endl;

	vSeq = { 3, 4, 5, 3, 2, 1 };
	check(vSeq);
	std::cout << "Passed tree 2." << std::endl;

	// A star-tree
	vSeq = { 8, 8, 8, 8, 8, 8 };
	check(vSeq);
	std::cout << "Passed star tree." << std::endl;

	// A chain-tree
	vSeq = { 2, 3, 4, 5, 6, 7 };
	check(vSeq);
	std::cout << "Passed chain tree." << std::endl;

	// All trees of 6 vertices
	int count = 0;
	vSeq = { 1, 1, 1, 1, 1 };
	while (true)
	{
		if (!check(vSeq))
		{
			std::cout << "Failed%^$!" << std::endl;
			break;
		}

		count++;
		if (!getNext(vSeq))
		{
			std::cout << "Passed "
				<< count
				<< " trees of "
				<< vSeq.size() + 2
				<< " vertices!!!!" << std::endl;
			break;
		}
	}
        
    return 0;
}

