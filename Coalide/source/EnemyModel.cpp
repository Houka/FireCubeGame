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
		_acorn = false;

		_destroyed = false;
		_sparky = false;

		_shooting = false;
		_dispersing = false;

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
 * Sets the texture for Nicoal based on angle facing and state
 *
 * @param angle  direction Nicoal facing in degrees
 */
void EnemyModel::setDirectionTexture(float angle, bool isAcorn){
    
    if(isAcorn){
        if(angle > ONE_ANGLE && angle <= TWO_ANGLE){
            Rect enemy_south = Rect(0.0f, 0.0f, 64.0f, 64.0f);
            _node->setPolygon(enemy_south);
        }
        else if(angle > TWO_ANGLE && angle <= FOUR_ANGLE){
            Rect enemy_west = Rect(64.0f, 0.0f, 64.0f, 64.0f);
            _node->setPolygon(enemy_west);
        }
        else if(angle > FOUR_ANGLE && angle <= SEVEN_ANGLE){
            Rect enemy_north = Rect(128.0f, 0.0f, 64.0f, 64.0f);
            _node->setPolygon(enemy_north);
        }
        else{
            Rect enemy_east = Rect(192.0f, 0.0f, 64.0f, 64.0f);
            _node->setPolygon(enemy_east);
        }
    }
    else {
        if(angle > ONE_ANGLE && angle <= TWO_ANGLE){
            Rect enemy_south = Rect(0.0f, 0.0f, 128.0f, 128.0f);
            _node->setPolygon(enemy_south);
        }
        else if(angle > TWO_ANGLE && angle <= FOUR_ANGLE){
            Rect enemy_west = Rect(128.0f, 0.0f, 128.0f, 128.0f);
            _node->setPolygon(enemy_west);
        }
        else if(angle > FOUR_ANGLE && angle <= SEVEN_ANGLE){
            Rect enemy_north = Rect(256.0f, 0.0f, 128.0f, 128.0f);
            _node->setPolygon(enemy_north);
        }
        else{
            Rect enemy_east = Rect(384.0f, 0.0f, 128.0f, 128.0f);
            _node->setPolygon(enemy_east);
        }
    }
}

/**
 * Returns true if the enough time has elapsed since the last sling
 */
bool EnemyModel::timeoutElapsed(){
    //wait between 2 and 5 seconds
	if (isMushroom()) {
		return Timestamp().ellapsedMillis(_previousTime) >= (SPORE_TIMEOUT - _rndTimerReduction);
	}

    return Timestamp().ellapsedMillis(_previousTime) >= (SLING_TIMEOUT - _rndTimerReduction);
}

void EnemyModel::updateSparks(bool visible) {
	_sparks->setVisible(visible);
}

void EnemyModel::updateSparks() {
	if (_sparks->isVisible()) {
		int frame = _sparks->getFrame();
		if (frame < 5) {
			_sparks->setFrame(frame + 1);
		}
		else {
			_sparks->setVisible(false);
			_sparks->setFrame(0);
		}
	}
}

void EnemyModel::animateSpore() {
	std::shared_ptr<AnimationNode> node = std::dynamic_pointer_cast<AnimationNode>(_node);
	int frame = node->getFrame();
	if (frame == 6) {
		setDestroyed();
		return;
	}
	if (frame < 4) {
		node->setFrame(4);
	}
	else {
		node->setFrame(frame + 1);
	}
}

Vec2 EnemyModel::getPosition() {
	if (isAcorn()) {
		return Vec2(CapsuleObstacle::getPosition().x+0.25, CapsuleObstacle::getPosition().y + 0.25);
	}
	else if (isOnion()) {
		return Vec2(CapsuleObstacle::getPosition().x, CapsuleObstacle::getPosition().y + 0.5);
	}
	else if (isMushroom()) {
		return Vec2(CapsuleObstacle::getPosition().x-0.5, CapsuleObstacle::getPosition().y + 0.5);
	}
	else {
		return CapsuleObstacle::getPosition();
	}
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
