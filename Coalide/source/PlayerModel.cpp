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
		setTextureKey("nicoal_nicoal");
		setBodyType(b2_dynamicBody);
        setLinearDamping(GLOBAL_AIR_DRAG);

		_node = nullptr;
        _color = Color4::WHITE;
        _charging = false;
        _sizePlayer = size;

		setDensity(1.0f);
		setRestitution(0.4f);
		setFixedRotation(true);

		_stunned = false;
		_onFire = false;

		_stunTimer = 0;

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
    return !_stunned && _body->GetLinearVelocity().Length() <= MAX_SPEED_FOR_SLING;
}

/**
 * Increments the stun timer and returns true if the character has finished being stunned.
 */
void PlayerModel::stillStunned() {
	_stunTimer++;
	_stunned = _stunTimer != STUN_TIME;
}

/**
* Returns true if player is in bounds
**/
bool PlayerModel::inBounds(int width, int height){
    b2Vec2 position = _body->GetPosition();
    CULog("Player Size: %s", _sizePlayer.toString().c_str());
    return ((position.x) > 0 && (position.y) > 0 && position.x < width && position.y < height);
}

/**
* Updates the aim arrow.
*
* @param node  updates the aim arrow.
*/
void PlayerModel::updateArrow(cugl::Vec2 aim, bool visible) {

	aim *= -.3;
	cugl::Vec2 playerImageOffset = cugl::Vec2(_node->getWidth() / 2.0, _node->getHeight() / 2.0);

	float scaleFactor = aim.length();
	float angle = -1 * aim.getAngle();

	_arrow->setScale(cugl::Vec2(scaleFactor, 1));
	_arrow->setAngle(angle);
	_arrow->setPosition(cugl::Vec2(playerImageOffset.x + aim.x, playerImageOffset.y - aim.y));
	_arrow->setVisible(visible);
	_arrow->setColor(cugl::Color4::RED);
}

void PlayerModel::updateArrow(bool visible) {
	_arrow->setVisible(visible);
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
