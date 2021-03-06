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
bool CollisionController::init(const std::shared_ptr<AssetManager>& assets) {
    _thud = assets->get<Sound>("thud");	return true;
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
	if (soA->getName() == "enemy") {
		EnemyModel* enemy = (EnemyModel*)soA;
		enemy->setCoalided(true);
        enemy->markCollisionTimeout();

		if (enemy->isSpore()) {
			enemy->setDispersing();
		}

		if (soB->getName() == "player") {
			PlayerModel* player = (PlayerModel*)soB;
            player->setCameraShakeAmplitude(1);
            if(! AudioEngine::get()->isActiveEffect("thud")) {
                AudioEngine::get()->playEffect("thud", _thud, false, _thud->getVolume());
            }
            player->setDirectionTexture(player->getPlayerDirection(), 5);
            player->setCoalided(true);
			
			if (enemy->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
				player->setSparky(true);
			}
			if (player->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
				enemy->setSparky(true);
			}

			if (enemy->isOnion() && !enemy->isStunned()) {
				player->stunOnStop(4000);
				if (!enemy->alreadyStopping() && enemy->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF))
					enemy->setShouldStop();
			}

			if (player->getLinearVelocity().length() > 9) {
				player->setSuperCollide(true); 
			}
		}

		if (enemy->isOnion() && !enemy->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF)) {
			enemy->stunEnemy(4500);
		}
	}

	if (soB->getName() == "enemy") {
		EnemyModel* enemy = (EnemyModel*)soB;
		enemy->setCoalided(true);
        enemy->markCollisionTimeout();

		if (enemy->isSpore()) {
			enemy->setDispersing();
		}

		if (soA->getName() == "player") {
			PlayerModel* player = (PlayerModel*)soA;
            player->setCameraShakeAmplitude(1);
            if(! AudioEngine::get()->isActiveEffect("thud")) {
                AudioEngine::get()->playEffect("thud", _thud, false, _thud->getVolume());
            }

            float angle = player->getLinearVelocity().getAngle();
            player->setCoalided(true);

			if (enemy->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
				player->setSparky(true);
			}
			if (player->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
				enemy->setSparky(true);
			}

			if (enemy->isOnion() && !enemy->isStunned()) {
				player->stunOnStop(4000);
                player->setDirectionTexture(player->getPlayerDirection(), 7);
				if (!enemy->alreadyStopping() &&
					enemy->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF))
					enemy->setShouldStop();
			}
			if (player->getLinearVelocity().length() > 9) {
				player->setSuperCollide(true);
			}
		}

        if (enemy->isOnion() && !enemy->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF)) {
            enemy->stunEnemy(4500);
        }
        
	}

	// player or enemy shoves something stationary
    if(soA->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF) && canBeShoved(soA)){
        if(soB->getName() == "player"){
            PlayerModel* player = (PlayerModel*) soB;
			player->setSparky(false);
            if(!player->alreadyStopping())
                player->setShouldStop();
        } else if(soB->getName() == "enemy") {
            EnemyModel* enemy = (EnemyModel*) soB;
			enemy->setSparky(false);
            if(!enemy->alreadyStopping())
                enemy->setShouldStop();
        }
    }


    if(soB->getLinearVelocity().isNearZero(SPECIAL_COLLISION_SPEED_CUTOFF) && canBeShoved(soB)){
        if(soA->getName() == "player"){
            PlayerModel* player = (PlayerModel*) soA;
			player->setSparky(false);
            if(!player->alreadyStopping()){
                player->setShouldStop();
            }
        } else if(soA->getName() == "enemy") {
            EnemyModel* enemy = (EnemyModel*) soA;
			enemy->setSparky(false);
            if(!enemy->alreadyStopping())
                enemy->setShouldStop();
        }
    }

	// Remove broken objects
	if (soA->getName() == BREAKABLE_NAME) {
		ObjectModel* object = (ObjectModel*)soA;
		object->setAnimating();
	}
	else if (soB->getName() == BREAKABLE_NAME) {
		ObjectModel* object = (ObjectModel*)soB;
		object->setAnimating();
	}
}

// determines if a simple obstacle can be shoved by a player or enemy
bool CollisionController::canBeShoved(SimpleObstacle* so) {
	return (so->getName() == "player" || so->getName() == "enemy" 
		|| so->getName() == "moveable" || so->getName() == "breakable");
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
