#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <GL/glut.h>
#include <unistd.h>
#include "vector3.h"
#include "halfedge.h"

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

/////////////////////////////////////////////////////
//	TEMP. GLOBALS
/////////////////////////////////////////////////////
vector<Face*> faces;
map< unsigned int, Vertex* > vertices;
map< pair<unsigned int, unsigned int>, HalfEdge* > edges;
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
/////////////////////////////////////////////////////
void readMeshFile(const string &fileName, map< pair<unsigned int, unsigned int>, HalfEdge*> &edges,
					map< unsigned int, Vertex* > &vertices, vector<Face*> &faces );

//All vertex points are printed given the map vertices
void print( const map<unsigned int, Vertex*> &vertices );

//All half edges are printed in the form of their vertex ids given the map edges
void print( const map< pair<unsigned int, unsigned int>, HalfEdge*> &edges);

//Testing function to check loops of a face given a starting halfedge
int test_cycle();

void GLinit(int argc, char** argv);
void GLrender();

//Given point (x,y), a pixel is drawn with colors r,g,b
void renderPixel(int x, int y, float r, float g, float b);

//Given points (x1,y1) and (x2,y2), a line is drawn with the colors r,g,b
void dda(int x1, int y1, int x2, int y2, float r = 1.0, float g = 1.0, float b = 1.0);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


int main( int argc, char* argv[] ){
	if( argc != 2 ){
		cout << "Usage: ./a.out [meshInput.txt]" << endl;
		return -1;
	}
	readMeshFile(argv[1], edges, vertices, faces);
	print(vertices);
	print(edges);

	GLinit(argc, argv);
	glutDisplayFunc(GLrender);
	glutMainLoop();

	return 0;
}


/////////////////////////////////////////////////////
//	FUNCTION IMPLEMENTATIONS
/////////////////////////////////////////////////////
void print( const map<unsigned int, Vertex*> &vertices ){
	map<unsigned int, Vertex*>::const_iterator it = vertices.begin();
	cout << "Vertices of mesh and their outgoing halfedge: " << endl;
	for( it; it != vertices.end(); ++it ){
		cout << it->first << ": " << it->second->coords << "\t" << it->second->edge->vert->coords << endl;
	}
	cout << endl;
}
void print( const map< pair<unsigned int, unsigned int>, HalfEdge*> &edges){
	map< pair<unsigned int, unsigned int>, HalfEdge*>::const_iterator it = edges.begin();
	cout << "Half edges of mesh: " << endl;
	for( it; it != edges.end(); ++it ){
		cout << it->first.first << "," << it->first.second << endl;
	}
}

int test_cycle(){
	char c = 'y';
	while( c == 'y' ){
		cout << "Enter a pair of numbers from 0 to " << vertices.size()-1 << ". (Must be unique, e.g: 0,1)" << endl;
		int x, y;
		cin >> x >> y;
		map< pair<unsigned int, unsigned int>, HalfEdge*>::iterator it = edges.find( pair<unsigned int,unsigned int>(x,y) );
		if( it == edges.end()){
			cerr << "Error: missing edge, this shouldn't be happening!" << endl;
			return(-1);
		}
		HalfEdge* starting_edge = edges[ pair<unsigned int, unsigned int>(x,y) ];
		HalfEdge* base_edge = starting_edge;
		do{
			cout << "Pointing to: " << starting_edge->vert->coords << endl;
			starting_edge = starting_edge->next;
		}
		while( starting_edge != base_edge );
		cout << "Test again? y/n: ";
		cin >> c;
	}
}

void readMeshFile(const string &fileName, map< pair<unsigned int, unsigned int>, HalfEdge*> &edges,
					map< unsigned int, Vertex* > &vertices, vector<Face*> &faces ){
	faces.clear();
	vertices.clear();
	edges.clear();

	vector<Vector3> faces_to_process;

	ifstream meshInput ( fileName.c_str(), ios_base::in);
	if( meshInput.is_open() ){
		float num_verts, num_faces;
		meshInput >> num_verts;
		meshInput >> num_faces;
		int i = 0;
		while( i < num_verts ){
			float x, y, z;
			meshInput >> x;
			meshInput >> y;
			meshInput >> z;
			vertices[ vertices.size()  ] = new Vertex(Vector3(x,y,z));
			//vertices.push_back( pair<Vector3, vector<Edge> >( Vector3(x,y,z), vector<Edge>() ));
			//vIndex[ Vector3(x,y,z) ] = vertices.size()-1;
			++i;
		}
		while( !meshInput.eof() ){
			int x, y, z;
			meshInput >> x;
			meshInput >> y;
			meshInput >> z;
			if( meshInput.eof() ) break;
			faces_to_process.push_back( Vector3(x,y,z) );
		}
		for( int i = 0; i < faces_to_process.size(); ++i ){
			int x = faces_to_process[i].x;
			int y = faces_to_process[i].y;
			int z = faces_to_process[i].z;
			HalfEdge* new_he_0 = new HalfEdge();
			faces.push_back( new Face(new_he_0));

			edges[ pair<unsigned int, unsigned int>(x,y) ] = new_he_0;
			edges[ pair<unsigned int, unsigned int>(x,y) ]->face = faces[faces.size()-1];
			edges[ pair<unsigned int, unsigned int>(x,y) ]->vert = vertices[y];

			edges[ pair<unsigned int, unsigned int>(y,z) ] = new HalfEdge();
			edges[ pair<unsigned int, unsigned int>(y,z) ]->face = faces[faces.size()-1];
			edges[ pair<unsigned int, unsigned int>(y,z) ]->vert = vertices[z];

			edges[ pair<unsigned int, unsigned int>(z,x) ] = new HalfEdge();
			edges[ pair<unsigned int, unsigned int>(z,x) ]->face = faces[faces.size()-1];
			edges[ pair<unsigned int, unsigned int>(z,x) ]->vert = vertices[x];

			if( !vertices[x]->edge ) vertices[x]->edge = edges[ pair<unsigned int, unsigned int>(x,y) ];
			if( !vertices[y]->edge ) vertices[y]->edge = edges[ pair<unsigned int, unsigned int>(y,z) ];
			if( !vertices[z]->edge ) vertices[z]->edge = edges[ pair<unsigned int, unsigned int>(z,x) ];

			//Setting next half edges of each half edge
			edges[ pair<unsigned int, unsigned int>(x,y) ]->next = edges[ pair<unsigned int, unsigned int>(y,z) ];
			edges[ pair<unsigned int, unsigned int>(y,z) ]->next = edges[ pair<unsigned int, unsigned int>(z,x) ];
			edges[ pair<unsigned int, unsigned int>(z,x) ]->next = edges[ pair<unsigned int, unsigned int>(x,y) ];
			
			//Setting prev half edges of each half edge
			edges[ pair<unsigned int, unsigned int>(x,y) ]->prev = edges[ pair<unsigned int, unsigned int>(z,x) ];
			edges[ pair<unsigned int, unsigned int>(y,z) ]->prev = edges[ pair<unsigned int, unsigned int>(x,y) ];
			edges[ pair<unsigned int, unsigned int>(z,x) ]->prev = edges[ pair<unsigned int, unsigned int>(y,z) ];

			//Setting the opposite pairs of each half edge
			if( edges.find( pair<unsigned int,unsigned int>(y,x)) != edges.end() ){
				edges[ pair<unsigned int, unsigned int>(x,y) ]->pair = edges[ pair<unsigned int, unsigned int>(y,x) ];
				edges[ pair<unsigned int, unsigned int>(y,x) ]->pair = edges[ pair<unsigned int, unsigned int>(x,y) ];
			}
			if( edges.find( pair<unsigned int,unsigned int>(z,y)) != edges.end() ){
				edges[ pair<unsigned int, unsigned int>(y,z) ]->pair = edges[ pair<unsigned int, unsigned int>(z,y) ];
				edges[ pair<unsigned int, unsigned int>(z,y) ]->pair = edges[ pair<unsigned int, unsigned int>(y,z) ];
			}
			if( edges.find( pair<unsigned int,unsigned int>(x,z)) != edges.end() ){
				edges[ pair<unsigned int, unsigned int>(z,x) ]->pair = edges[ pair<unsigned int, unsigned int>(x,z) ];
				edges[ pair<unsigned int, unsigned int>(x,z) ]->pair = edges[ pair<unsigned int, unsigned int>(z,x) ];
			}
		}
	}
}

void GLinit(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// ...
	// Complete this function
	// ...
	glutCreateWindow("CS 130 - Hao Nguyen");

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
}

void GLrender()
{
	dda(200,200, 400, 400);
	glutSwapBuffers();
}

void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
	glColor3f(r,g,b);
	glVertex2i(x,y);
	glEnd();
}

void dda(int x1, int y1, int x2, int y2, float r, float g, float b)
{
	if(x1 > x2){
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	float slope = (float)(y2 - y1)/(float)(x2 - x1);

	if( -1.0 < slope && slope < 1.0 )
	{	
		float j = y1;
		for( float i = x1; i < x2; ++i ){
			renderPixel(i, j);
			j+=slope;
		}
	}
	else if( slope <= -1.0 ){
		float j = x1;
		for( float i = y1; i >= y2; --i ){
			renderPixel(j, i);
			j-=1/slope;
		}
	}
	else
	{
		if( x1 == x2 ) slope = 0.0;
		float j = x1;
		for( float i = y1; i <= y2; i++ ){
			renderPixel(j, i);
			if(slope != 0.0) j+= 1/slope;
		}
	}
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
