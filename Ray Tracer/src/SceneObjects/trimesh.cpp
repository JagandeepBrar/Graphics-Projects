#include <cmath>
#include <float.h>
#include "trimesh.h"

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt )
        return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    faces.push_back( newFace );
    scene->add(newFace);
    return true;
}

char *
Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

bool triangleInsideOutside(const Vec3d &a, const Vec3d &b, const Vec3d &c, Vec3d &q, Vec3d &n) {
	//Gets the dot product of the cross product and n for each point
	//If each is above zero, it means it is within the triangle
	if (
        (((c - b) ^ (q - b))*n) < RAY_EPSILON ||
        (((a - c) ^ (q - c))*n) < RAY_EPSILON ||
        (((b - a) ^ (q - a))*n) < RAY_EPSILON) 
    {
		return false;
	}
	
    return true;
}

// Calculates and returns the normal of the triangle too.
bool TrimeshFace::intersectLocal( const ray& r, isect& i ) const
{
    //Local variables
    const Vec3d 
        &a = parent->vertices[ids[0]],
        &b = parent->vertices[ids[1]],
        &c = parent->vertices[ids[2]];
    Vec3d   
        dir = r.getDirection(),
        pos = r.getPosition(),
        n = ((b-a)^(c-a)),
        q;
    double
        t;
    //Ensure that the denominator isn't 0
    if((n*dir) == 0) {
        return false;
    }
    //Calculate the t, and get the intersection point on the supporting plane
	n.normalize();
    t = ((a-pos)*n)/(n*dir);
    if(t < RAY_EPSILON) {
        return false;
    }
    //Get the intersection point
    q = r.at(t);
	//Check if the intersection point is within the triangle
    if(triangleInsideOutside(a,b,c,q,n)) {
		//Set the time, normal, and object in isect
        i.setT(t);
        i.setN(n);
        i.obj = this;
		//Return true
        return true;
    }
    //Return false on no intersection
    return false;
}

void
Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
        Vec3d a = vertices[(**fi)[0]];
        Vec3d b = vertices[(**fi)[1]];
        Vec3d c = vertices[(**fi)[2]];
        
        Vec3d faceNormal = ((b-a) ^ (c-a));
		faceNormal.normalize();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
}

