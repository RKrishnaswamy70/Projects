#ifndef GENERIC_HEAP_H

#include <vector>

//=============================================================================
// A classic Heap template.
//
// An array is associated with a balanced binary tree. Each element of the
// array is a node of the tree.  The children of the node at index i are at 
// indices 2i+1 and 2i.  For example an array:
//     Index  0   1   2   3   4   5   6   7
//     Value  9   4   5   3   8   2   6   7
// is associated with the balanced binary tree:
//                     9
//                     |
//              4---------------5
//              |               |
//         3---------8     2----------6
//         |
//     7----
//
// A 'Heap' is a special array, where the associated binary tree satisfies
// the 'Heap property', a transitive, asymmetric relation on the values of
// the nodes.  We use the >= property.  The above array is not a heap since
// the nodes with values 4, 5 and 3 are not >= values at their children. 
// Below is a 'heaped' rearrangement of the above array:
//     Index  0   1   2   3   4   5   6   7
//     Value  9   8   7   3   4   6   5   2
// is associated with the balanced binary tree:
//                     9
//                     |
//              8---------------7
//              |               |
//         3---------4     6----------5
//         |
//     2----
//
// GenericHeap extends std::vector, and provides an operation to initialize
// it as a heap.  It also provides some heap-specific operations.
//=============================================================================
template
<class T>
class GenericHeap : public std::vector<T>
{
public:
	// Constructor
	GenericHeap()
	{}


	// This is a key method of GenericHeap.  It examines the subtree
	// rooted at index i.  It assumes that every node of this subtree
	// satisfies the heap-property except the subtree-root at i.
	//
	// The heap property requires the value at every node to be >= the
	// value at its two children.  This method examines the subtree of
	// the heap rooted at i, i.e. the subtree S comprising elements at:
	//    i
	//    2i+1, 2i+2 (children of i)
	//    4i+3, 4i+4 (children of 2i+1)
	//    4i+5, 4i+6 (children of 2i+2)
	//    ...
	//
	// It assumes that every node in the subtree except the root (at i)
	// satisfies the heap property.  However, the root may not, and it
	// may be less than one or both of its children.  Hence it needs to 
	// be 'reheaped'.
	//
	// The algorithm comprises 'dribbling' it down to the node where it 
	// satisfies the heap property.  This may cause it to be dribbled
	// down all the way to a leaf.
	void reheap(size_t i)
	{
		size_t cur;
		size_t left;
		size_t right;
		size_t largest;
		size_t count = this->size();

		// We use the std::vector::at() to access the i'th element of
		// the underlying array.
		cur = i;
		T value = this->at(cur);
		while (true)
		{
			// Invariant: at(cur) == value
			left = 2 * cur + 1;
			right = left + 1;

			// The code below ensures that at(largest) will be
			// the largest of at(cur), at(left) and at(right).
			largest = cur;
			if ((left < count) && (this->at(left) >= this->at(largest)))
			{
				largest = left;
			}

			if ((right < count) && (this->at(right) >= this->at(largest)))
			{
				largest = right;
			}

			if (cur == largest)
			{
				// We are done.  Exit
				break;
			}

			// Exchange at(cur) and at(largest). Note at(cur) == value.
			this->at(cur) = this->at(largest);
			this->at(largest) = value;

			// Now move cur to largest, and continue.
			cur = largest;
		}
	}


	// Establishes the heap property for the elements of this vector.
	void initialize()
	{
		// Make *this into a heap.  The reheaping is a decrement loop
		// as we need to reheap from the bottom of the tree.  Also,
		// as the elements from at size()/2 .. size() are all leaves
		// of the heap, we reheap from size()/2 down to 0.
		for (int i = (int)this->size() / 2; i >= 0; i--)
		{
			reheap(i);
		}
	}


	// Extracts the greatest element of the heap.  This is at index 0.
	const T& getGreatest()
	{
		return this->at(0);
	}


	// Deletes the largest element of the heap.  Note that the largest
	// element is at index 0.
	void deleteGreatest()
	{
		// The first element (which is the largest) is replaced by
		// the last element.
		this->at(0) = this->at(this->size() - 1);

		// Now the last element is removed.  This is ok since
		// it has been copied at index 0.
		this->pop_back();

		// Now the element at index zero (the previous last element)
		// probably violates the heap property of being >= values at
		// its children.  So reheap.
		reheap(0);
	}


	// Replaces the greatest element of the heap.  Note that the 
	// greatest element is at index 0.
	void replaceGreatest(const T& value)
	{
		// The first element (which is the greatest) is replaced by
		// value.
		this->at(0) = value;

		// Now the element at index zero (value) probably violates
		// the heap property of being >= values at its children.  
		// So reheap.
		reheap(0);
	}
};

#endif // GENERIC_HEAP_H
