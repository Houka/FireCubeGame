//
//	CollisionController.cpp
//	Coalide
//
#include "CollisionController.h"
#include "TileModel.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>

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
void CollisionController::beginContact(b2Contact* contact) { }

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
void CollisionController::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2Body* bodyA = fixtureA->GetBody();
	b2Body* bodyB = fixtureB->GetBody();

	SimpleObstacle* soA = (SimpleObstacle*)(bodyA->GetUserData());
	SimpleObstacle* soB = (SimpleObstacle*)(bodyB->GetUserData());

	if (soB->getName() == TILE_NAME) {
		if (soA->getName() == PLAYER_NAME) {
			((PlayerModel*)soA)->getFrictionJoint()->SetMaxForce(((TileModel*)soB)->getFriction());
		}
		else if (soB->getName() == ENEMY_NAME) {
			((EnemyModel*)soA)->getFrictionJoint()->SetMaxForce(((TileModel*)soB)->getFriction());
		}
		contact->SetEnabled(false);
	}

	if (soA->getName() == TILE_NAME) {
		if (soB->getName() == PLAYER_NAME) {
			((PlayerModel*)soB)->getFrictionJoint()->SetMaxForce(((TileModel*)soA)->getFriction());
		}
		else if (soB->getName() == ENEMY_NAME) {
			((EnemyModel*)soB)->getFrictionJoint()->SetMaxForce(((TileModel*)soA)->getFriction());
		}
		contact->SetEnabled(false);
	}
}


void CollisionController::endContact(b2Contact* contact) { }
