//
// PlayerModel.cpp
// Coalide
//

#include "PlayerModel.h"
#include "Constants.h"

#define IMPULSE_SCALE .05
#define COLLISION_TIMEOUT 0
#define MAX_SPEED_FOR_SLING 2


/**
* Initializes a new player with the given position and size.
*
* @param  pos		Initial position in world coordinates
* @param  size   	The dimensions of the sprite.
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool PlayerModel::init(const Vec2 & pos, const Size & size) {
	if (CapsuleObstacle::init(pos, size)) {
		setName(PLAYER_NAME);
		setTextureKey(PLAYER_TEXTURE);
		setBodyType(b2_dynamicBody);

		_node = nullptr;
        _color = Color4::WHITE;

		setDensity(1.0f);
		setRestitution(0.4f);
		setFixedRotation(true);

		return true;
	}
	return false;
}

void PlayerModel::dispose() {
	_node = nullptr;
	_frictionJoint = nullptr;
}

/**
* Applies the impulse to the body of this player
*/
void PlayerModel::applyLinearImpulse(Vec2& impulse) {
    _body->ApplyLinearImpulseToCenter(IMPULSE_SCALE * b2Vec2(impulse.x,impulse.y), true);
}

/**
 * Returns true if the player is moving slow enough to sling
 */
bool PlayerModel::canSling(){
    return _body->GetLinearVelocity().Length() <= MAX_SPEED_FOR_SLING;
}

/**
* Updates the object's physics state (NOT GAME LOGIC). This is the method
* that updates the scene graph and is called after collision resolution.
*
* @param dt Timing values from parent loop
*/
void PlayerModel::update(float dt) {
	Obstacle::update(dt);
	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawscale);
		_node->setAngle(getAngle());
        if(!canSling()){
            _node->setColor(Color4::RED);
        } else {
            _node->setColor(_color);
        }
	}
    if(_shouldStopSoon && Timestamp().ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT){
        _shouldStopSoon = false;
        _body->SetLinearVelocity(b2Vec2(0,0));
    }
}
