// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

static Vec3d
	vecZero = Vec3d(0,0,0),
	vecOne = Vec3d(1,1,1);

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
Vec3d RayTracer::trace( double x, double y )
{
	// Clear out the ray cache in the scene for debugging purposes,
	scene->intersectCache.clear();

    ray r( vecZero, vecZero, ray::VISIBILITY );

    scene->getCamera().rayThrough( x,y,r );
	Vec3d ret = traceRay( r, vecOne, traceUI->getDepth());
	ret.clamp();
	return ret;
}

Vec3d RayTracer::traceRay( const ray& r, const Vec3d& thresh, int depth )
{
	Material
		material;
	isect 
		i;
	Vec3d
		direction = r.getDirection(),
		position = r.getPosition(),
		reflVec,
		refrVec,
		shade,
		N,
		R,
		q;
	bool
		refl,
		refr;
	double
		cosine,
		sine,
		snells,
		nindex,
		tindex;
	if( scene->intersect(r, i)) {
		//Get the material, and find the shade
		material = i.getMaterial();
		shade = material.shade(scene,r,i);
		//Check the depth level of the recursion
		if(depth <= 0) {
			return shade;
		}
		//If the depth isn't reached, continue to check if it has refractions or reflections
		refl = (!material.kr(i).iszero());
		refr = (!material.kt(i).iszero());
		//Get intersection vector and normal and cos
		q = r.at(i.t);
		N = i.N;
		cosine = N*direction;
		//Handles reflection
		if(refl) {
			//Gets the value to calculate the reflection ray
			//Calculate the reflection ray vector,and normalize
			R = (direction-(2*cosine*N));
			R.normalize();
			//Make a ray object out of this data
			ray reflRay(q, R, ray::REFLECTION);
			//Recursively call traceRay() for reflections
			//Get the product of the return and the material reflection property
			reflVec = prod((traceRay(reflRay,thresh,(depth-1))), material.kr(i));
		} else {
			//No reflection -> zero vector
			reflVec = vecZero;
		}
		//Handles refraction
		if(refr) {
			//Check if it is exiting, entering, or total internal reflection
			if(cosine < -RAY_EPSILON) {
				//Entering
				nindex = 1.0;
				tindex = material.index(i);
			} else if(cosine > RAY_EPSILON) {
				//Exiting
				nindex = material.index(i);
				tindex = 1.0;
				N = -i.N;
			} else {
				//Total Internal, just return with no refraction
				return shade+reflVec;
			}
			//Calculate sine using our cosine
			sine = sqrt(1 - (pow(cosine, 2)));
			//Calculate snell's law
			snells = asin((nindex/tindex)*sine);
			//Calculate the directional vector, normalize 
			R = (direction-(N-(snells*N)));
			R.normalize();
			//Generate the refraction ray
			ray refrRay(q, R, ray::REFRACTION);
			//Recursively call traceRay() for refractions
			//Get the product of the return and the material transmissive property
			refrVec = prod((traceRay(refrRay,thresh,(depth-1))), material.kt(i));
		} else {
			//No refraction -> zero vector
			refrVec = vecZero;
		}
		return shade+reflVec+refrVec;
	} else {
		return vecZero;
	}
}

RayTracer::RayTracer()
	: scene( 0 ), buffer( 0 ), buffer_width( 256 ), buffer_height( 256 ), m_bBufferReady( false )
{
}


RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn )
{
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos )
		path = ".";
	else
		path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( ! sceneLoaded() )
		return false;

	if(traceUI->getOctreeStatus()) {
		return scene->octreeAssemble();
	}
	
	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];

	}
	memset( buffer, 0, w*h*3 );
	m_bBufferReady = true;

}

void RayTracer::tracePixel( int i, int j )
{
	Vec3d col;

	if( ! sceneLoaded() )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( x,y );
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}
