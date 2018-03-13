//
//	EnemyModel.cpp
//	Coalide
//

#include "EnemyModel.h"
#include "Constants.h"
#include <cugl/util/CUTimestamp.h>

#define MAX_SPEED_FOR_SLING 2
#define IMPULSE_SCALE 8
#define SLING_TIMEOUT 5000
#define COLLISION_TIMEOUT 0

using namespace cugl;


/**
* Initializes a new enemy with the given position and size.
*
* @param  pos		Initial position in world coordinates
* @param  size   	The dimensions of the sprite.
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool EnemyModel::init(const Vec2 & pos, const Size & size) {
    std::srand (time(NULL));
	if (CapsuleObstacle::init(pos, size)) {
		setName(ENEMY_NAME);
		setTextureKey(ENEMY_TEXTURE);
		setBodyType(b2_dynamicBody);
		
		_node = nullptr;

		setDensity(1.0f);
		setRestitution(0.4f);
		setFixedRotation(true);

		_previousTime = Timestamp();
		_rndTimerReduction = std::rand() % 1000;

		return true;
	}
    
	return false;
}

void EnemyModel::dispose() {
	_node = nullptr;
	_frictionJoint = nullptr;
}

/**
 * Applies the impulse to the body of this enemy
 */
void EnemyModel::applyLinearImpulse(Vec2& impulse) {
    _previousTime.mark();
    _rndTimerReduction = std::rand() % 1000;
    _body->ApplyLinearImpulseToCenter(IMPULSE_SCALE * b2Vec2(impulse.x,impulse.y), true);
}

/**
* Returns true if the player is moving slow enough to sling
*/
bool EnemyModel::canSling(){
    return _body->GetLinearVelocity().Length() <= MAX_SPEED_FOR_SLING;
}

/**
 * Returns true if the enough time has elapsed since the last sling
 */
bool EnemyModel::timeoutElapsed(){
    //wait between 4 and 5 seconds
    return Timestamp().ellapsedMillis(_previousTime) >= (SLING_TIMEOUT - _rndTimerReduction);
	//return Timestamp().ellapsedMillis(_previousTime) >= (SLING_TIMEOUT);
}

/**
* Updates the object's physics state (NOT GAME LOGIC). This is the method
* that updates the scene graph and is called agter collision resolution.
*
* @param dt Timing values from parent loop
*/
void EnemyModel::update(float dt) {
	Obstacle::update(dt);
	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawscale);
		_node->setAngle(getAngle());
	}
    if(!canSling()){
        _node->setColor(Color4::RED);
    } else {
        _node->setColor(Color4::WHITE);
    }
    if(_shouldStopSoon && Timestamp().ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT){
        _shouldStopSoon = false;
        _body->SetLinearVelocity(b2Vec2(0,0));
    }
}
