//
//	CollisionController.cpp
//	Coalide
//
#include "CollisionController.h"
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include <Box2D/Dynamics/b2World.h>

using namespace cugl;


#pragma mark -
#pragma mark Constructors
bool CollisionController::init() {
	return true;
}

void CollisionController::dispose() { }

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
void CollisionController::beginContact(b2Contact* contact) {
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    SimpleObstacle* soA = (SimpleObstacle*)(bodyA->GetUserData());
    SimpleObstacle* soB = (SimpleObstacle*)(bodyB->GetUserData());

	if ((soA->getName() == "player" || soA->getName() == "enemy") && soB->getName() == "tile") {
		b2FrictionJointDef* jointDef;
		
		jointDef->localAnchorA.SetZero();
		jointDef->localAnchorB.SetZero();
		jointDef->bodyA = bodyA;
		jointDef->bodyB = bodyB;
		jointDef->maxForce = 10;
		jointDef->maxTorque = 10;
		jointDef->collideConnected = true;
		
		_jointsToCreate.push_back((b2JointDef*)jointDef);
	}
}

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
void CollisionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) { }


void CollisionController::endContact(b2Contact* contact) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	SimpleObstacle* soA = (SimpleObstacle*)(fixtureA->GetUserData());
	SimpleObstacle* soB = (SimpleObstacle*)(fixtureB->GetUserData());

	if ((soA->getName() == "player" || soA->getName() == "enemy") && soB->getName() == "tile") {
		b2Body* bodyA = fixtureA->GetBody();
		b2Body* bodyB = fixtureB->GetBody();
		
		b2JointEdge* joints = bodyA->GetJointList();
		_jointsToDestroy.push_back(joints[0].joint);
	}
}


void CollisionController::addDestroyFrictionJoints(std::shared_ptr<ObstacleWorld> world) {
	if (_jointsToCreate.size() > 0) {
		for (int i = 0; i < _jointsToCreate.size(); i++) {
			world->getWorld()->CreateJoint(_jointsToCreate[i]);
		}
		_jointsToCreate.clear();
	}
	
	if (_jointsToDestroy.size() > 0) {
		for (int i = 0; i < _jointsToDestroy.size(); i++) {
			world->getWorld()->DestroyJoint(_jointsToDestroy[i]);
		}
		_jointsToDestroy.clear();
	}
}
