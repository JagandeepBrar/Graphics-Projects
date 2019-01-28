#include <cmath>

#include "light.h"

static Vec3d
	vecZero = Vec3d(0,0,0),
	vecOne = Vec3d(1,1,1);

using namespace std;

double DirectionalLight::distanceAttenuation( const Vec3d& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation( const Vec3d& P ) const
{
	Vec3d
		L = getDirection(P),
		k = vecOne;
	isect
		i;
	ray
		r = ray(P, L, ray::SHADOW);
	bool
		intersect = scene->intersect(r,i);
	//See if the ray intersects the plane
	if(intersect) {
		//If there is a material at the intersection, recursively run shadowAttenuation on this material
		if(i.material != nullptr) {
			k = prod(i.getMaterial().kt(i),shadowAttenuation(r.at(i.t)));
		}
		//Return the product of the material transmissive property and the k value
		return prod(k, i.getMaterial().kt(i));
	}
	//When there is no shadow, just return the colour
	return color;
}

Vec3d DirectionalLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d DirectionalLight::getDirection( const Vec3d& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const Vec3d& P ) const
{
	double distance = (P-position).length();
	return (min(1.0,(1/(
		constantTerm +
		linearTerm*distance +
		quadraticTerm*pow(distance,2))))
	);
}

Vec3d PointLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d PointLight::getDirection( const Vec3d& P ) const
{
	Vec3d ret = position - P;
	ret.normalize();
	return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
	Vec3d
		L = getDirection(P),
		k = vecOne;
	isect
		i;
	ray
		r = ray(P, L, ray::SHADOW);
	bool
		intersects = scene->intersect(r, i);
	double
		dIntersection = (P-r.at(i.t)).length(),
		dPosition = (P-position).length();
	if(
		//See if the ray intersects the plane
		intersects && 
		//Also ensures that the distance to the light position is greater than the intersection distance
		(dPosition > dIntersection)
	) {
		//If there is a material at the intersection, recursively run shadowAttenuation on this material
		if (i.material != nullptr) {
			k = prod(i.getMaterial().kt(i),shadowAttenuation(r.at(i.t)));
		}
		//Return the product of the material transmissive property and the k value
		return prod(k, i.getMaterial().kt(i));
	}
	//When there is no shadow, just return the colour
	return color;
}
