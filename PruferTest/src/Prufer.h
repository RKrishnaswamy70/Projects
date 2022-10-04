#include <list>
#include <set>
#include <climits>

//====================================================================
// The class Prufer is intended to convert between a sequence
// and edge representation of a tree.  This header defines graph
// definitions, and the class.
//
// Motivation: Represent a Tree by a Sequence
// A tree of n vertices can be represented in one of two ways:
//   - As a sequence of n-1 edges.
//   - As a numeric sequence of length n-2.
//
// The Prufer sequence theorem states that if T is a tree of n-1 edges
// on vertices {v1,...,vn}, then:
//   1. It can be represented by a sequence of n-2 vertices.  Vertices
//         may be repeated arbitrarily often in the sequence.
//   2. The degree of each vertex is one plus the number of times it
//         occurs in the sequence.
//   3. The outermost vertices of the tree (degree-1 vertices) are
//         the vertices *not* in the representing sequence.  This is
//         a consequence of 2.
//
// To determine the Prufer sequence representing a tree, we need to
// define a 'Tail' of a tree.
//
// Definition: Tail of a tree.
// A tail of a tree is an edge (v1, v2) of the tree where v1 is a degree-1
// vertex, and is the greatest such vertex.  Denote the tail of tree t by
// tail(t).
//
// Observation: Deleting the tail(t) from t leaves a tree on the
// vertices excluding the degree-1 vertex of tail(t).
//
// To generate a Prufer Sequence from a tree t:
//   1. Let tail(t) = (v1,v2), v1 be the degree-1 vertex, and let
//      t' the the tree with tail(t) erased.
//   2. v2 is the first element of the Prufer sequence.
//   3. The rest of the sequence is obtained by steps 1..3 for t'.
//   4. Stop the above steps when the tree has a single edge.
//
// So for example the tree:
//      1----2-----3
//           |
//           4
// Has:
//          t            tail(t)     Prufer
//  -----------------------------------------
//   (1,2)(3,2),(4,2)      (4,2)       2
//   (1,2)(3,2)            (3,2)       2
//   (1,2)
// So the Prufer sequence is: 2 2
//
// It can be shown that there is a one-one correspondence between Prufer
// sequences and trees.  In particular, there is an inverse of the above
// algorithm that generates a tree from a prufer sequence.
//
// The class Prufer provides both conversions.  It can be constructed
// from a tree or a sequence, and it can yeild either a tree or a
// sequence.  When a tree is provided, the set of vertices must be
// a contiguous set {1..n}, and n-1 edges are provided.  When a sequence
// is provided, it must be of length n-2 from integers of {1..n}.
//====================================================================

//--------------------------------------------------------------------
// Common graph definitions
//--------------------------------------------------------------------

// A vertex is denoted by an integer, and an edge by a pair
// of vertices
typedef size_t Vertex;
enum { UndefinedVertex = INT_MAX };

typedef std::pair<Vertex, Vertex> Edge;

// A graph is a list of undirected edges
typedef std::list<Edge> Graph;

// A vertex set and a vertex sequence
typedef std::set<Vertex> VertexSet;
typedef std::list<Vertex> VertexSequence;

//--------------------------------------------------------------------
// The class Prufer for conversion between trees with n vertices
// and n-2 long sequences
//--------------------------------------------------------------------
class Prufer
{
public:
	// Constructor from a tree
	// @param: tree The tree of n vertices.  This is just a list
	// of edges.  They must use all the vertices of {1..n}, and
	// there must be n-1 of them.
	Prufer(const Graph& tree);

	// Constructor from a sequence
	// @param seq A sequence of length n-2 of integers from {1..n}
	// representing a tree of n vertices.
	Prufer(const VertexSequence& seq);

	// Gets the tree.
	const Graph& getTree();

	// Gets the sequence
	const VertexSequence& getSequence();

private:
	Graph m_tree;
	VertexSequence m_seq;
};