//
// scene.h
//
// The Scene class and the geometric types that it can contain.
//

#pragma warning (disable: 4786)


#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <memory>

#include "ray.h"
#include "material.h"
#include "camera.h"

#include "../vecmath/vec.h"
#include "../vecmath/mat.h"

class Light;
class Scene;

class SceneElement
{
public:
	virtual ~SceneElement() {}

	Scene *getScene() const { return scene; }

	// For debugging purposes, draws using OpenGL
	virtual void glDraw(int quality, bool actualMaterials, bool actualTextures) const  { }

protected:
	SceneElement( Scene *s )
		: scene( s ) {}

    Scene *scene;
};

class BoundingBox
{
public:
	Vec3d min;
	Vec3d max;

	void operator=(const BoundingBox& target);

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox &target) const;
	
	// does the box contain this point?
	bool intersects(const Vec3d& point) const;

	// if the ray hits the box, put the "t" value of the intersection
	// closest to the origin in tMin and the "t" value of the far intersection
	// in tMax and return true, else return false.
	bool intersect(const ray& r, double& tMin, double& tMax) const;
};

class TransformNode
{
protected:

    // information about this node's transformation
    Mat4d    xform;
	Mat4d    inverse;
	Mat3d    normi;

    // information about parent & children
    TransformNode *parent;
    std::vector<TransformNode*> children;
    
public:
   	typedef std::vector<TransformNode*>::iterator          child_iter;
	typedef std::vector<TransformNode*>::const_iterator    child_citer;

    ~TransformNode()
    {
        for(child_iter c = children.begin(); c != children.end(); ++c )
            delete (*c);
    }

    TransformNode *createChild(const Mat4d& xform)
    {
        TransformNode *child = new TransformNode(this, xform);
        children.push_back(child);
        return child;
    }
    
    // Coordinate-Space transformation
    Vec3d globalToLocalCoords(const Vec3d &v)
    {
        return inverse * v;
    }

    Vec3d localToGlobalCoords(const Vec3d &v)
    {
        return xform * v;
    }

    Vec4d localToGlobalCoords(const Vec4d &v)
    {
        return xform * v;
    }

    Vec3d localToGlobalCoordsNormal(const Vec3d &v)
    {
        Vec3d ret = normi * v;
		ret.normalize();
		return ret;
    }

	const Mat4d& transform() const		{ return xform; }

protected:
    // protected so that users can't directly construct one of these...
    // force them to use the createChild() method.  Note that they CAN
    // directly create a TransformRoot object.
    TransformNode(TransformNode *parent, const Mat4d& xform )
        : children()
    {
        this->parent = parent;
        if (parent == NULL)
            this->xform = xform;
        else
            this->xform = parent->xform * xform;
        
        inverse = this->xform.inverse();
        normi = this->xform.upper33().inverse().transpose();
    }
};

class TransformRoot : public TransformNode
{
public:
    TransformRoot()
        : TransformNode(NULL, Mat4d()) {}
};

// A Geometry object is anything that has extent in three dimensions.
// It may not be an actual visible scene object.  For example, hierarchical
// spatial subdivision could be expressed in terms of Geometry instances.
class Geometry
	: public SceneElement
{
public:
    // intersections performed in the global coordinate space.
    bool intersect(const ray&r, isect&i) const;
    
protected:
    // intersections performed in the object's local coordinate space
    // do not call directly - this should only be called by intersect()
	virtual bool intersectLocal( const ray& r, isect& i ) const = 0;

public:
	virtual bool isTrimeshObject() const { return false; }
	virtual bool hasBoundingBoxCapability() const;
	const BoundingBox& getBoundingBox() const { return bounds; }
	virtual void ComputeBoundingBox()
    {
        // take the object's local bounding box, transform all 8 points on it,
        // and use those to find a new bounding box.

        BoundingBox localBounds = ComputeLocalBoundingBox();
        
        Vec3d min = localBounds.min;
		Vec3d max = localBounds.max;

		Vec4d v, newMax, newMin;

		v = transform->localToGlobalCoords( Vec4d(min[0], min[1], min[2], 1) );
		newMax = v;
		newMin = v;
		v = transform->localToGlobalCoords( Vec4d(max[0], min[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(min[0], max[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(max[0], max[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(min[0], min[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(max[0], min[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(min[0], max[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( Vec4d(max[0], max[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		
		bounds.max = Vec3d(newMax);
		bounds.min = Vec3d(newMin);
    }

    // default method for ComputeLocalBoundingBox returns a bogus bounding box;
    // this should be overridden if hasBoundingBoxCapability() is true.
    virtual BoundingBox ComputeLocalBoundingBox() { return BoundingBox(); }

    void setTransform(TransformNode *transform) { this->transform = transform; };
    
	Geometry( Scene *scene ) 
		: SceneElement( scene ) {}

	// For debugging purposes, draws using OpenGL
	void glDraw(int quality, bool actualMaterials, bool actualTextures) const;

	// The defult does nothing; this is here because it is not required
	// that you implement this function if you create your own scene objects.
	virtual void glDrawLocal(int quality, bool actualMaterials, bool actualTextures) const { }

protected:
	BoundingBox bounds;
    TransformNode *transform;
};

// A SceneObject is a real actual thing that we want to model in the 
// world.  It has extent (its Geometry heritage) and surface properties
// (its material binding).  The decision of how to store that material
// is left up to the subclass.
class SceneObject
	: public Geometry
{
public:
	virtual const Material& getMaterial() const = 0;
	virtual void setMaterial( Material *m ) = 0;

	void glDraw(int quality, bool actualMaterials, bool actualTextures) const;

protected:
	SceneObject( Scene *scene )
		: Geometry( scene ) {}
};

// A simple extension of SceneObject that adds an instance of Material
// for simple material bindings.
class MaterialSceneObject
	: public SceneObject
{
public:
	virtual ~MaterialSceneObject() { delete material; }

	virtual const Material& getMaterial() const { return *material; }
	virtual void setMaterial( Material* m )	{ delete material; material = m; }

protected:
	MaterialSceneObject( Scene *scene, Material *mat ) 
		: SceneObject( scene ), material( mat ) {}

	Material* material;
};

/*
 * ENTERING OCTREE CODE
 */

template <class T>
class OcTree {
private:
	//Easy to access constant vectors
	const Vec3d
		vecZero = Vec3d(0,0,0),
		vecOne = Vec3d(1,1,1);
	//Constants used in octree assembly
	const int
		minObjects = 5;
	//Pointers and object vector
	std::vector<T*> objects;
	std::unique_ptr<OcTree<T>> subnodes[8];
	std::unique_ptr<BoundingBox> box;
public:
	/**
	 * Basic Constructor: Initializes all of its subnodes, and position
	 */
    OcTree(std::vector<T*> &o, int depth) {
		//Inits subnodes, bounding box
		initSubnodes();
		initBoundingBox();
		//Before we continue, we make sure we haven't reached our depth threshold
		if(depth <= 0) {
			objects = o;
			return;
		}
		setBoundingBox(o);
		objects = o;
    }

	/**
	 * initSubnodes(): Sets all the subnodes to NULL
	 */
	void initSubnodes() {
		for(int i=0; i<8; i++) {
			subnodes[i] = NULL;
		}
	}

	void initBoundingBox() {
		box = std::unique_ptr<BoundingBox>(new BoundingBox());
		box->min = vecZero;
		box->max = vecZero;
	}

	void setBoundingBox(std::vector<T*> &o) {
		for(int x=0; x<(int)o.size(); x++) {
			for(int y=0; y<3; y++) {
				if(o[x]->getBoundingBox().max[y] > box->max[y]) {
					box->max[y] = o[x]->getBoundingBox().max[y];
				}
				if(o[x]->getBoundingBox().min[y] < box->min[y]) {
					box->min[y] = o[x]->getBoundingBox().min[y];
				}
			}
		}
	}

	bool intersect(const ray &r, isect &i) {
		typedef
			vector<Geometry*>::const_iterator iter;
		double 
			tMin = -1.0e308, // 1.0e308 is close to infinity... close enough for us!
			tMax = 1.0e308;  // Both of these are reassigned in BoundingBox::intersect, so I just copied the values
		bool
			found = false;
		if(box->intersect(r,tMin,tMax)) {
			if(isLeaf()) {
				for(iter j = objects.begin(); j != objects.end(); j++) {
					isect cur;
					if ((*j)->intersect(r, cur)) {
						if(!found || cur.t < i.t) {
							i = cur;
							found = true;
						}
					}
				}
			} else {
				for(int x=0; x<8; x++) {
					if(!found) {
						found = subnodes[x]->intersect(r,i);
					}
					
				}
			}
		}
		return found;
	}

	/*
	 * isLeaf(): Checks the first subnode, if it's NULL, then this ocnode must be a leaf
	 */
	bool isLeaf() {
		return subnodes[0] == NULL;
	}
};

/**
 * ENDING OCTREE CODE
 */

class Scene
{
public:
	typedef std::vector<Light*>::iterator 			liter;
	typedef std::vector<Light*>::const_iterator 	cliter;

	typedef std::vector<Geometry*>::iterator 		giter;
	typedef std::vector<Geometry*>::const_iterator cgiter;

    TransformRoot transformRoot;

public:
	Scene() 
		: transformRoot(), objects(), lights()
		{}
	virtual ~Scene();

	void add( Geometry* obj )
	{
		obj->ComputeBoundingBox();
		objects.push_back( obj );
	}
	void add( Light* light )
	{ lights.push_back( light ); }

	bool intersect( const ray& r, isect& i ) const;

	// OCTREE
	bool bruteforceIntersect(const ray &r, isect &i) const;
	bool octreeIntersect(const ray &r, isect &i) const;
	bool octreeAssemble();

	std::vector<Light*>::const_iterator beginLights() const { return lights.begin(); }
	std::vector<Light*>::const_iterator endLights() const { return lights.end(); }
        
	const Camera& getCamera() const		{ return camera; }
	Camera& getCamera()					{ return camera; }

	// For efficiency reasons, we'll store texture maps in a cache
	// in the Scene.  This makes sure they get deleted when the scene
	// is destroyed.
	TextureMap* getTexture( string name );

	// These two functions are for handling ambient light; in the Phong model,
	// the "ambient" light is considered a property of the _scene_ as a whole
	// and hence should be set here.
	Vec3d ambient() const	{ return ambientIntensity; }
	void addAmbient( const Vec3d& ambient ) { ambientIntensity += ambient; }

	void glDraw(int quality, bool actualMaterials, bool actualTextures) const;

	const BoundingBox& bounds() const		{ return sceneBounds; }

private:
    std::vector<Geometry*> objects;
	std::vector<Geometry*> nonboundedobjects;
	std::vector<Geometry*> boundedobjects;
    std::vector<Light*> lights;
	std::unique_ptr<OcTree<Geometry>> octree;
    Camera camera;
	//OcTree<Geometry> octree;

	// This is the total amount of ambient light in the scene
	// (used as the I_a in the Phong shading model)
	Vec3d ambientIntensity;

	typedef std::map< std::string, TextureMap* > tmap;
	tmap textureCache;
	
	// Each object in the scene, provided that it has hasBoundingBoxCapability(),
	// must fall within this bounding box.  Objects that don't have hasBoundingBoxCapability()
	// are exempt from this requirement.
	BoundingBox sceneBounds;


public:
	// This is used for debugging purposes only.
	mutable std::vector< std::pair<ray, isect> > intersectCache;
};

#endif // __SCENE_H__
