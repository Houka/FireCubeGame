//
//	CollisionController.cpp
//	Coalide
//
#include "CollisionController.h"
#include "TileModel.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "ObjectModel.h"
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>

using namespace cugl;

/** Max speed of object A for object B to stop on collision - happens to be
 * the same as speed for sling input
 */
#define SPECIAL_COLLISION_SPEED_CUTOFF 2.0

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
	
    if(soA->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF)){
        if(soB->getName() == "player"){
            CULog("should be stopping");
            PlayerModel* player = (PlayerModel*) soB;
            if(!player->alreadyStopping())
                player->setShouldStop();
        } else if(soB->getName() == "enemy") {
            EnemyModel* enemy = (EnemyModel*) soB;
            if(!enemy->alreadyStopping())
                enemy->setShouldStop();
        }
    }
    if(soB->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF)){
        if(soA->getName() == "player"){
            PlayerModel* player = (PlayerModel*) soA;
            if(!player->alreadyStopping())
                player->setShouldStop();
        } else if(soA->getName() == "enemy") {
            EnemyModel* enemy = (EnemyModel*) soA;
            if(!enemy->alreadyStopping())
                enemy->setShouldStop();
        }
    }

	// Remove broken objects
	if (soA->getName() == BREAKABLE_NAME) {
		ObjectModel* object = (ObjectModel*)soA;
		object->setBroken();
	}
	else if (soB->getName() == BREAKABLE_NAME) {
		ObjectModel* object = (ObjectModel*)soB;
		object->setBroken();
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


void CollisionController::endContact(b2Contact* contact) { }
