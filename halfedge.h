#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "vector3.h"
#include <map>

struct HalfEdge;
struct Vertex;
struct Face;

struct HalfEdge{
	Vertex* vert;
	Face* face;
	HalfEdge* next;
	HalfEdge* prev;
	HalfEdge* pair;

	HalfEdge()
		: vert(NULL), face(NULL), next(NULL), prev(NULL), pair(NULL)
	{}
};

struct Vertex{
	Vector3 coords;
	HalfEdge* edge;
	
	Vertex()
		: coords(Vector3()), edge(NULL)
	{}

	Vertex( Vector3 v )
		: coords(v), edge(NULL)
	{}
};

struct Face{
	HalfEdge* edge;

	Face()
		: edge(NULL)
	{}

	Face(HalfEdge* he)
		: edge(he)
	{}
};

#endif
