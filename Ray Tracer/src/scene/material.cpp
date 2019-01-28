#include "ray.h"
#include "material.h"
#include "light.h"

#include "../fileio/imageio.h"

using namespace std;
extern bool debugMode;



Vec3d Material::shade(Scene *scene, const ray& r, const isect& i) const
{
    double
        t = i.t,
        distanceAtt,
        NL;
    Vec3d
		emissive,
        ambient,
        diffuse,
        specular,
        attenuation,
        shadowAtt,
        N = i.N,
        L,
        V,
        R,
        q = r.at(t);
	//Fetch the emissive light component as the base
	emissive = ke(i);
    //Fetch the ambient light and get the product of the material value
    ambient = prod(scene->ambient(),ka(i));
    for(
        vector<Light *>::const_iterator iterator = scene->beginLights();
        iterator != scene->endLights();
        iterator++
    ) {
		//Fetch values from the light source, including the direction and attenuations
        L = (*iterator)->getDirection(q);
        distanceAtt = (*iterator)->distanceAttenuation(q);
        shadowAtt = (*iterator)->shadowAttenuation(q);
		//Calculate the attenuations
        attenuation = distanceAtt*shadowAtt;
		//Calculate values for the diffuse and specular components
        NL = max(0.00, (N*L));
        R = ((2*NL*N)-L);
        V = (scene->getCamera().getEye()-q);
        V.normalize();
        //Calculate the diffuse and specular components
        diffuse += (prod((kd(i)*NL),attenuation));
        specular += (prod(ks(i)*pow(max(0.00,V*R),shininess(i)),attenuation));
    }
	//Return the summation of all three components
    return(emissive+ambient+diffuse+specular);
}


TextureMap::TextureMap( string filename )
{
    data = load( filename.c_str(), width, height );
    if( 0 == data )
    {
        width = 0;
        height = 0;
        string error( "Unable to load texture map '" );
        error.append( filename );
        error.append( "'." );
        throw TextureMapException( error );
    }
}

Vec3d TextureMap::getMappedValue( const Vec2d& coord ) const
{
	if (data == 0)
		return Vec3d(1, 1, 1);
    //Get x and y through coord (unit square val) and image width and height
	double x = coord[0]*width;
	double y = coord[1]*height;
    //Get location of the RGB colors
    //Quick reverse engineering the imageio::load method
    int loc = (((floor(y))*3*(width))+(floor(x)*3));
    //Create and return the colour vector
	return Vec3d(
        (data[loc+0]/255.0), 
        (data[loc+1]/255.0), 
        (data[loc+2]/255.0)
    );
}


Vec3d TextureMap::getPixelAt( int x, int y ) const
{
    // This keeps it from crashing if it can't load
    // the texture, but the person tries to render anyway.
    if (0 == data)
        return Vec3d(1.0, 1.0, 1.0);

    if( x >= width )
        x = width - 1;
    if( y >= height )
        y = height - 1;

    // Find the position in the big data array...
    int pos = (y * width + x) * 3;
    return Vec3d( double(data[pos]) / 255.0, 
        double(data[pos+1]) / 255.0,
        double(data[pos+2]) / 255.0 );
}

Vec3d MaterialParameter::value( const isect& is ) const
{
    if( 0 != _textureMap )
        return _textureMap->getMappedValue( is.uvCoordinates );
    else
        return _value;
}

double MaterialParameter::intensityValue( const isect& is ) const
{
    if( 0 != _textureMap )
    {
        Vec3d value( _textureMap->getMappedValue( is.uvCoordinates ) );
        return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
    }
    else
        return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}

