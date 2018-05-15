//
//	EnemyModel.cpp
//	Coalide
//

#include "EnemyModel.h"
#include "Constants.h"
#include <cugl/util/CUTimestamp.h>

#define MAX_SPEED_FOR_SLING 2
#define IMPULSE_SCALE 8
#define SLING_TIMEOUT 4000
#define SPORE_TIMEOUT 6000
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
	if (CapsuleObstacle::init(pos, size)) {
		setName(ENEMY_NAME);
		setBodyType(b2_dynamicBody);
        setLinearDamping(GLOBAL_AIR_DRAG);

		_node = nullptr;

		setDensity(4.8f);
		setRestitution(0.4f);
		setFixedRotation(true);

		_stunned = false;
		_onFire = false;

		_wandering = false;
		_targeting = true;

		_mushroom = false;
		_spore = false;
		_onion = false;

		_destroyed = false;

		_waterInbetween = false;

		_previousTime = Timestamp();
        unsigned int rnd_seed = (unsigned int) (100 * pos.x + pos.y);
        std::srand(rnd_seed);
		_rndTimerReduction = std::rand() % 2000;

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
    _rndTimerReduction = std::rand() % 3000;
    _body->SetLinearVelocity(b2Vec2(impulse.x,impulse.y));
}

/**
* Returns true if the player is moving slow enough to sling
*/
bool EnemyModel::canSling(){
    return _body->GetLinearVelocity().Length() <= MAX_SPEED_FOR_SLING;
}

/**
 * Returns true if enemy is in bounds
 **/
bool EnemyModel::inBounds(int width, int height){
    b2Vec2 position = _body->GetPosition();
    return (position.x > 0 && position.y > 0 && position.x < width && position.y < height);
}

/**
 * Returns true if the enough time has elapsed since the last sling
 */
bool EnemyModel::timeoutElapsed(){
    //wait between 2 and 5 seconds
	if (isMushroom()) {
		return Timestamp().ellapsedMillis(_previousTime) >= (SPORE_TIMEOUT - _rndTimerReduction);
	}
	/*CULog(to_string(Timestamp().ellapsedMillis(_previousTime)).c_str());
	CULog(to_string((SLING_TIMEOUT - _rndTimerReduction)).c_str());*/
    return Timestamp().ellapsedMillis(_previousTime) >= (SLING_TIMEOUT - _rndTimerReduction);
}

Vec2 EnemyModel::getPosition() {
	return Vec2(CapsuleObstacle::getPosition().x, CapsuleObstacle::getPosition().y + 0.25);
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
    auto ts = Timestamp();
    if(_stunned){
        _node->setColor(Color4::GREEN);
        if(ts.ellapsedMillis(_stunTimeout) >= _stunDuration) {
            setStunned(false);
        }
    }
    else if(!canSling() && !isSpore()){
        _node->setColor(Color4::RED);
    } else {
        _node->setColor(Color4::WHITE);
    }
    if(_shouldStopSoon && ts.ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT){
        _shouldStopSoon = false;
        _body->SetLinearVelocity(b2Vec2(0,0));
    }
    /*if(!isMushroom() && _waterInbetween && ts.ellapsedMillis(_noLineOfSiteTimeout) >= 2000){
        _previousTime.mark();
    }*/
}
