//
// PlayerModel.cpp
// Coalide
//

#include "PlayerModel.h"

void PlayerModel::dispose() { }

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
		std::string name("player");
		setName(name);
		setTextureKey("protosheet_player");
		setBodyType(b2_dynamicBody);

		_node = nullptr;

		setDensity(1.0f);
		setFriction(0.1f);
		setRestitution(0.4f);

		return true;
	}
	return false;
}

/**
* Applies the force to the body of this player
*/
void PlayerModel::applyForce() { }

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
	}
}
