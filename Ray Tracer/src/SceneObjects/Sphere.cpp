#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
    //Sphere is at the origin (0,0,0) and has a radius of 1
    //Define local variables
    double  
        a,
        b, 
        c, 
        x, 
        t1, 
        t2;
    Vec3d   
        q,
        d = r.getDirection(),
        p = r.getPosition();
    //Determine the value under the square root of the equation
    a = (
        pow(d[0], 2) +
        pow(d[1], 2) +
        pow(d[2], 2)
    );
    b = 2*(
        p[0]*d[0] +
        p[1]*d[1] +
        p[2]*d[2]
    );
    c = (
        pow(p[0], 2) +
        pow(p[1], 2) +
        pow(p[2], 2) -
        1
    );
    //Determine the discriminant
    x = (pow(b,2)-(4*a*c));
    //Checks if the discriminant is negative, return false if so (zero intersections)
    if(x < RAY_EPSILON) {
        return false;
    }
    //Calculate the two t values
    x = sqrt(x);
    t1 = (-b+x)/(2*a);
    t2 = (-b-x)/(2*a);
    //If t1 is less than 0 (or RAY_EPSILON), it means both intersections are behind the viewer
    if(t1 < RAY_EPSILON) {
        return false;
    } else {
        if(t2 < RAY_EPSILON) {
            //If t2 is negative, we can't use it (t2 is behind, so t is t1
            i.setT(t1);
        } else {
            //if t2 isn't negative, it will always be lower than t1, so use it
            i.setT(t2);
        }
        //Get the vector of the intersection at t, normalize it
        q = r.at(i.t);
        q.normalize();
		//Check if the vector is inside the object, and if so return false
		if (q*d > RAY_EPSILON) {
			return false;
		}
        //Set the normal in isect, set the isect obj to this, and return true
        i.setN(q);
        i.obj = this;
        return true;
    }
    //Return false on no intersection
    return false;
}
