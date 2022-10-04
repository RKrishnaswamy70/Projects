#include <set>
#include <map>
#include <iostream>

#include "Prufer.h"
#include "GenericHeap.h"

//=============================================================================
// Overview
//
// Suppose
//    V is a set of vertices represented by integers
//    s(m) is a sequence of length m from vertices of V 
//    t is a tree on V
//
// Suppose functions tailf and tailg are defined as below.
//    tailf(t) = (v1,v2) where (v1,v2) is an edge of t, and
//       v1 is the greatest vertex of t with degree 1.
//       tailf(t) is not defined for a single edge tree
//       (i.e. m = 2).
//    tailg(V, s(m)) = (w1,w2) where w1 is the greatest
//       vertex not in s(m), and w2 is the last vertex of s(m).
//       tailg(V, s(0)) is not defined for m=2.
//
// For a tree t, let f(t) = (V, s(n-2)) be  defined as follows
// Let:
//   - tailf(t)=(v1,v2)
//   - t' = t - {tailf(t)}
//   - f(t') = (V', s'(n-3))
// Then
//   - V = {v1} + V'
//   - s = s' . v2
// For the one-edge tree t0= {(v,w)} , the tailf(t) is not defined,
//   - V = {v,w}
//   - s = empty-sequence
//
// Likewise for a pair (V, s(n-2)) of a set of n-2 vertices
// and a sequence of length n-2 from s (repetitions allowed),
// let g(V,s(n-2)) = t be defined as follows.
// Let
//   - tailg(V,s(n-2)) = (v1,v2)
//   - V' = V - {v1}
//   - s'(n-3) = s(n-3) (i.e. the last element of s(n-2) is erased)
//   - g(V' s'(n-3)) = t'
// Then
//   - t = t' + {(v1,v2)}
// For the 2 vertex set V0, and empty-seqeunce we define
//   - t = {(v1,v2)})
// where V0 = {v1,v2}.
//
// Clearly for V having 2 vertices, t is just a single edge
// {(v1,v2)}, and s(2-2) = s(0) is the empty sequence.  Thus
//     f(t) = (V, empty-sequence)
//     g(V, empty-sequence) = f(t)
//
// It can be shown by induction that if V has n vertices then
//     f(t) = (V,s) iff g(V,s) = t
//     If n>2 then tailf(t) = tailg(V,s)
// This is the basis of the Prufer conversion algorithm.  
//
// Prufer uses two builder classes to assist in constructing a tree from a
// sequence and conversely:
//   - PruferBuilder : To construct a prufer sequence from a tree.
//   - TreeBuilder   : To construct a tree from a prufer sequence.
// Both these classes use instantiations of GenericHeap.  PruferBuilder
// uses a GenericHeap of edges, and TreeBuilder uses a GenericHeap of
// vertices.
//=============================================================================

//=============================================================================
// Instantiations of GenericHeap for a VertexHeap and an EdgeHeap
//=============================================================================

// A GenericHeap of edges needs a >= operator.  This >n operator
// assumes that e.first is a vertex of degree 1 (an outermost vertex).  It
// returns true if the outermost vertex of the left edge is >= the 
// outermost vertex of the right edge.
bool operator >=(const Edge& eLeft, const Edge& eRight)
{
	return (eLeft.first >= eRight.first);
}

// A heap of edges
typedef GenericHeap<Edge> EdgeHeap;

// A heap of vertices.  Standard >= operator on Vertices
// is the >= for size_t.
typedef GenericHeap<Vertex> VertexHeap;

//=============================================================================
// PruferBuilder is state needed to construct the prufer sequence
// from a tree.  It contains a map from vertices to sets of vertices
// that they are incident on.
//
// Additionally, it has an heap of the outermost edges.  These are edges
// which have a degree one vertex.
//
// It provides an operation to extract the 'tail' of the tree, the edge
// incident on the greatest outermost vertex (degree 1 vertex).  This method
// removes the edge from the tree, and updates the incidence map.
//=============================================================================
typedef std::set<Vertex> VertexSet;

class PruferBuilder
{
public:
	// Constructor from a tree.  The tree is non-const and
	// will be whittled down one tail at a time by calls
	// to extractTail.
	PruferBuilder(const Graph& tree);

	// Extracts the tail from the tree and return the removed tail.
	Edge extractTail();

private:
	// The IncidenceMap keeps track of the set of vertices 
	// that are incident on a given vertex.
	typedef std::map<Vertex, VertexSet> IncidenceMap;

	// The incidence map for this tree.
	IncidenceMap m_incidenceMap;

	// The outermost edges are an edge heap.
	EdgeHeap m_outermostEdges;
};


PruferBuilder::PruferBuilder(const Graph& tree)
{
	for (Edge e : tree)
	{
		VertexSet& adjacentV1 = m_incidenceMap[e.first];
		VertexSet& adjacentV2 = m_incidenceMap[e.second];

		// Populate *this by adding e.second to the vertex set of 
		// the first vertex, and e.first to the vertex set of the
		// second vertex.
		adjacentV1.insert(e.second);
		adjacentV2.insert(e.first);
	}

	// ne is a 'normalized' edge with ne.first being the 
	// degree one vertex.
	Edge ne;
	for (IncidenceMap::value_type& mapEntry : m_incidenceMap)
	{
		// mapEntry is a pair (vertex, set-of-adjacent-vertices)
		if (mapEntry.second.size() == 1)
		{
			// mapEntry.first is a degree-1 vertex.
			ne.first = mapEntry.first;
			ne.second = *(mapEntry.second.begin());
			m_outermostEdges.push_back(ne);
		}
	}

	// Finally make m_outermostEdges a heap.
	m_outermostEdges.initialize();
}


// Extracts the tail from the tree, and returns the removed tail.
Edge PruferBuilder::extractTail()
{
	// Extract the greatest outermost edge.
	Edge tail = m_outermostEdges.getGreatest();

	// We can erase tail.first from the incidence map, as it is
	// the degree one vertex, and we are extracting the tail.
	m_incidenceMap.erase(tail.first);

	// As tail.first is being removed, we need to erase it from the
	// set of tail.second.
	VertexSet& s = m_incidenceMap[tail.second];
	s.erase(tail.first);

	if (s.size() == 1)
	{
		// We now have a new outermost edge, from tail.second to the
		// single vertex in s.
		Edge ne;
		ne.first = tail.second;
		ne.second = *(s.begin());
		m_outermostEdges.replaceGreatest(ne);
	}
	else
	{
		// Simply erase the greatest.
		m_outermostEdges.deleteGreatest();
	}

	return tail;
}


//=============================================================================
// TreeBuilder is state needed to construct the tree from a prufer sequence.
// It contains the prufer sequence, and a 'cardinality map' which maps each
// vertex to the number of times it occurs in the sequence.
//
// Additionally it has a heap of 'outermost vertices'.  These are vertices
// which are not in the sequence.
//
// It provides an operation to extract the 'tail' of the sequence, an edge
// defined by vertices (v1,v2) where v1 is the greatest vertex *not* in the
// sequence, and v2 is the first vertex of the sequence.
//
// The invariant is that the sequence vertices are the internal vertices
// of the associated tree.  Hence, the set of vertices *not* in the sequence
// is the set of outermost vertices, and the tail is the edge from the greatest
// outermost vertex to the start of the sequence.
//
// The extractTail() operation will remove the vertices of the tail from the
// sequence and the cardinality map and return the tail.
//=============================================================================
class TreeBuilder
{
public:
	TreeBuilder(const VertexSequence& seq);

	// The 'tail' of the TreeBuilder is an edge (v1,v2) where v1 is the greatest
	// vertex of the set of vertices which is *not* in the prufer sequence,
	// and v2 is the head of the sequence.
	Edge extractTail();

	// Extract the last edge of the tree.  At this point, the prufer
	// sequence should be empty, and there should only be two vertices.
	Edge extractLastEdge();

private:
	// Copy of the sequence
	VertexSequence m_seq;

	// Iterator into the sequence
	VertexSequence::iterator m_iter;

	// A map of the number of times a vertex occurs in m_vSeq
	typedef std::map<Vertex, size_t> CardinalityMap;

	// The cardinality map for this sequence.
	CardinalityMap m_card;

	// The outermost vertices
	VertexHeap m_outermostVertices;
};


TreeBuilder::TreeBuilder(const VertexSequence& seq)
{
	// The prufer sequence is 2 less than the number of vertices,
	// and all the numbers in it are from 1..numVertices.
	size_t numVertices = seq.size() + 2;

	// Initialize m_seq and m_card
	CardinalityMap::iterator cardIter;
	VertexSequence::const_iterator endIter = seq.end();
	for (VertexSequence::const_iterator iter = seq.begin();
		 iter != endIter;
		 iter++)
	{
		const Vertex& v = *iter;

		// Add vertex to m_seq
		m_seq.push_back(v);

		// Set the cardinality
		cardIter = m_card.find(v);
		if (cardIter == m_card.end())
		{
			// First occurrence in m_seq
			m_card[v] = 1;
		}
		else
		{
			// Another occurrence in m_seq
			cardIter->second++;
		}
	}

	// m_iter is the start of m_seq
	m_iter = m_seq.begin();

	// Outermost vertices are those not in m_card
	for (Vertex v = 1; v <= numVertices; v++)
	{
		cardIter = m_card.find(v);

		if (cardIter == m_card.end())
		{
			m_outermostVertices.push_back(v);
		}
	}

	// Finally make m_outermostVertices a heap.
	m_outermostVertices.initialize();
}



// The 'tail' of the TreeBuilder is an edge (v1,v2) where v1 is the 
// greatest vertex of the set of vertices which is *not* in the prufer
// sequence, and v2 is the head of the sequence.
Edge TreeBuilder::extractTail()
{
	// Extract the greatest outermost edge.
	Edge tail;
	
	// tail.first is the greatest in the heap
	tail.first = m_outermostVertices.getGreatest();

	// tail.second is the first of the rest of the sequence.
	tail.second = *m_iter;

	// Update m_iter for the next call to extractTail().
	m_iter++;

	CardinalityMap::iterator cardIter = m_card.find(tail.second);
	if (cardIter->second == 1)
	{
		// Eliminate this entry as the last occurrence of tail.second
		// has been removed from consideration.
		m_card.erase(cardIter);

		// Replace the greatest in m_outermostVertices
		m_outermostVertices.replaceGreatest(tail.second);
	}
	else
	{
		// Decrement the cardinality, as one occurrence of tail.second
		// has been removed from consideration.
		cardIter->second--;

		// Delete the greatest in m_outermostVertices
		m_outermostVertices.deleteGreatest();
	}

	return tail;
}


// Extract the last edge of the tree
Edge TreeBuilder::extractLastEdge()
{
	if (m_iter != m_seq.end())
	{
		throw std::logic_error("Sequence should be empty.");
	}
	else if (m_outermostVertices.size() != 2)
	{
		throw std::logic_error("There should only be two vertices left.");
	}

	Edge tail;
	tail.first = m_outermostVertices[0];
	tail.second = m_outermostVertices[1];

	m_outermostVertices.clear();

	return tail;
}


//=============================================================================
// The class Prufer
//=============================================================================

// Constructor from a tree
// @param: tree The tree of n vertices
Prufer::Prufer(const Graph& tree)
{
	// Assign the tree member
	m_tree = tree;

	// Build the prufer sequence member
	Edge tail;
	size_t treeSize = tree.size();
	PruferBuilder pBuilder(tree);

	m_seq.clear();
	for (size_t i = 1; i < treeSize; i++)
	{
		tail = pBuilder.extractTail();

		// Push back the second of the tail
		m_seq.push_back(tail.second);
	}
}


// Constructor from a sequence
// @param seq A sequence of length m representing a tree of m+2
//   vertices.  Each value of the sequence must be 1..(m+2).
Prufer::Prufer(const VertexSequence& seq)
{
	// Assign the prufer sequence member
	m_seq = seq;

	// Build the tree member
	Edge tail;
	size_t seqSize = seq.size();
	TreeBuilder pBuilder(seq);

	m_tree.clear();
	for (size_t i = 1; i <= seqSize; i++)
	{
		tail = pBuilder.extractTail();
		m_tree.push_back(tail);
	}

	tail = pBuilder.extractLastEdge();
	m_tree.push_back(tail);
}


// Gets the tree.
const Graph& Prufer::getTree()
{
	return m_tree;
}


// Gets the sequence
const VertexSequence& Prufer::getSequence()
{
	return m_seq;
}


