//
// PlayerModel.cpp
// Coalide
//

#include "PlayerModel.h"
#include "Constants.h"
#include <math.h>       /* pow */


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
	if (CapsuleObstacle::init(pos, size, Orientation::VERTICAL)) {
		setName(PLAYER_NAME);
		setTextureKey("nicoal");
		setBodyType(b2_dynamicBody);
        setLinearDamping(GLOBAL_AIR_DRAG);

		_node = nullptr;
        _color = Color4::WHITE;
        _charging = false;
        _sizePlayer = size;

		setDensity(4.0f);
		setRestitution(0.5f);
		setFixedRotation(true);

		_stunned = false;
        _stunOnStop = false;
		_onFire = false;
		_superCollide = false;
		_superCollideTimer = 0;
		_sparky = false;
		_fell = false;
		_isSliding = false;
		_charging = false;

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
* Returns true if player is in bounds
**/
bool PlayerModel::inBounds(int width, int height){
    b2Vec2 position = _body->GetPosition();
    return (position.x >= 0 && position.y >= 0 && position.x < width && position.y < height);
}


bool PlayerModel::isSuperCollide() {
	if (_superCollide && _superCollideTimer > 0) {
		_superCollideTimer -= 1;
	}
	else {
		_superCollide = false;
	}
	return _superCollide;
}

/**
 * Sets the texture for Nicoal based on angle facing and state
 *
 * @param angle  direction Nicoal facing in degrees
 * @param mode   the state of Nicoal (standing, sliding, chargning, etc)
 */
void PlayerModel::setDirectionTexture(float angle, int mode){
    float row_texture = 512.0f - (mode * 64.0f);
    
    if(angle > ONE_ANGLE && angle <= TWO_ANGLE){
        Rect nicoal_south = Rect(0.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south);
        player_direction = 0;
    }
    else if(angle > TWO_ANGLE && angle <= THREE_ANGLE){
        Rect nicoal_south_west = Rect(64.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south_west);
        player_direction = 1;
    }
    else if(angle > THREE_ANGLE && angle <= FOUR_ANGLE){
        Rect nicoal_west = Rect(128.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_west);
        player_direction = 2;
    }
    else if(angle > FOUR_ANGLE && angle <= FIVE_ANGLE){
        Rect nicoal_north_west = Rect(192.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north_west);
        player_direction = 3;
    }
    else if(angle > FIVE_ANGLE && angle <= SIX_ANGLE){
        Rect nicoal_north = Rect(256.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north);
        player_direction = 4;
    }
    else if(angle > SIX_ANGLE && angle <= SEVEN_ANGLE){
        Rect nicoal_north_east = Rect(320.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north_east);
        player_direction = 5;
    }
    else if(angle > SEVEN_ANGLE && angle <= EIGHT_ANGLE){
        Rect nicoal_east = Rect(384.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_east);
        player_direction = 6;
    }
    else{
        Rect nicoal_south_east = Rect(448.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south_east);
        player_direction = 7;
    }
}

void PlayerModel::setDirectionTexture(int dir, int mode){
    float row_texture = 512.0f - (mode * 64.0f);
    
    if(dir == 0){
        Rect nicoal_south = Rect(0.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south);
        player_direction = 0;
    }
    else if(dir == 1){
        Rect nicoal_south_west = Rect(64.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south_west);
        player_direction = 1;
    }
    else if(dir == 2){
        Rect nicoal_west = Rect(128.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_west);
        player_direction = 2;
    }
    else if(dir == 3){
        Rect nicoal_north_west = Rect(192.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north_west);
        player_direction = 3;
    }
    else if(dir == 4){
        Rect nicoal_north = Rect(256.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north);
        player_direction = 4;
    }
    else if(dir == 5){
        Rect nicoal_north_east = Rect(320.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_north_east);
        player_direction = 5;
    }
    else if(dir == 6){
        Rect nicoal_east = Rect(384.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_east);
        player_direction = 6;
    }
    else{
        Rect nicoal_south_east = Rect(448.0f, row_texture, 64.0f, 64.0f);
        _node->setPolygon(nicoal_south_east);
        player_direction = 7;
    }
}

/**
* Updates the aim arrow.
*
* @param node  updates the aim arrow.
*/
void PlayerModel::updateArrow(cugl::Vec2 aim, std::shared_ptr<Node> currNode, bool visible) {
    aim *= -.3;
	//float scaleFactor = aim.length();
    float angle = -1 * aim.getAngle();
    angle += 2.35619;

	//_arrow->setScale(cugl::Vec2(scaleFactor, 1));
	//_arrow->setAngle(angle);
    
    _arrow->setAngle(angle);
    _arrow->setPosition(currNode->getWorldPosition());
	_arrow->setVisible(visible);
	//_arrow->setColor(cugl::Color4::RED);
}

void PlayerModel::updateCircle(cugl::Vec2 aim, std::shared_ptr<Node> currNode, bool visible) {
    aim *= -.2875;
    cugl::Vec2 playerImageOffset = cugl::Vec2(_node->getWidth() / 2.0, _node->getHeight() / 2.0);
    //float scaleFactor = aim.length();
    //float angle = -1 * aim.getAngle();
    //angle += 2.35619;
    
    //_arrow->setScale(cugl::Vec2(scaleFactor, 1));
    //_arrow->setAngle(angle);
    
    //_circle->setAngle(angle);
    Vec2 scalingNumber = aim.scale(0.12f) * powf(aim.length(), 1.15f);
    _circle->setPosition(cugl::Vec2(currNode->getWorldPosition().x + scalingNumber.x, currNode->getWorldPosition().y - scalingNumber.y - (currNode->getHeight() / 2.0f)));
    _circle->setVisible(visible);
    //_arrow->setColor(cugl::Color4::RED);
}

void PlayerModel::updateArrow(bool visible) {
	_arrow->setVisible(visible);
}

void PlayerModel::updateCircle(bool visible) {
    _circle->setVisible(visible);
}

void PlayerModel::updateSparks(bool visible) {
	_sparks->setVisible(visible);
}

void PlayerModel::updateSparks() {
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


Vec2 PlayerModel::getPosition() {
	return Vec2(CapsuleObstacle::getPosition().x, CapsuleObstacle::getPosition().y + 0.25);
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
        if(_stunned){
//            _node->setColor(Color4::GREEN);
        }
        else {
//            _node->setColor(_color);
        }
	}
    if(_shouldStopSoon && Timestamp().ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT){
        _shouldStopSoon = false;
        _body->SetLinearVelocity(b2Vec2(0,0));
    }
    if(_stunOnStop && getLinearVelocity().length() <= MAX_SPEED_FOR_SLING){
        _stunOnStop = false;
        _stunned = true;
        _stunTimeout.mark();
    }
    if(_stunned && Timestamp().ellapsedMillis(_stunTimeout) >= _stunDuration){
        _stunned = false;
    }
    
    _cameraShakeAmplitude *= .9;
    if(_cameraShakeAmplitude < .01)
        _cameraShakeAmplitude = 0;
}
