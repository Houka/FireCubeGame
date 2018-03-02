//
//	CollisionController.h
//	Coalide
//
#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>

using namespace cugl;


class CollisionController {
public:
#pragma mark -
#pragma mark Constructors
	CollisionController() { }

	~CollisionController() { dispose(); }

	void dispose();

	bool init();


#pragma mark -
#pragma mark Collision Handling
	/**
	* Processes the start of a collision
	*
	* This method is called when we first get a collision between two objects.
	* We use this method to test if it is the "right" kind of collision.  In
	* particular, we use it to test if we make it to the win door.
	*
	* @param  contact  The two bodies that collided
	*/
	void beginContact(b2Contact* contact);

	/**
	* Handles any modifications necessary before collision resolution
	*
	* This method is called just before Box2D resolves a collision.  We use
	* this method to implement sound on contact, using the algorithms outlined
	* in Ian Parberry's "Introduction to Game Physics with Box2D".
	*
	* @param  contact  The two bodies that collided
	* @param  contact  The collision manifold before contact
	*/
	void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
};
#endif /* __COLLISION_CONTROLLER_H__ */
