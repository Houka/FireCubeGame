//
//	EnemyModel.cpp
//	Coalide
//

#include "EnemyModel.h"

void EnemyModel::dispose() { }

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
		_node = nullptr;

		setDensity(0.1f);
		setFriction(0.1f);
		setRestitution(0.4f);

		return true;
	}
	return false;
}

/**
* Applies the force to the body of this enemy
*/
void EnemyModel::applyForce() { }

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
}
