//
// TileMode.cpp
// Coalide
//

#include "TileModel.h"

using namespace cugl;


/**
* Initializes a new floor tile with the given position and size.
*
* @param  pos      Initial position in world coordinates
* @param  size   	The dimensions of the crate.
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool TileModel::init(const Vec2 & pos, const Size & size) {
	if (BoxObstacle::init(pos, Vec2(size.width, size.height))) {
		setName(TILE_NAME);
		setBodyType(b2_staticBody);
		return true;
	}
	return false;
}

/**
* Destroys this floor tile, releasing all resources.
*/
void TileModel::dispose() { }

/**
* Updates the object's physics state (NOT GAME LOGIC). This is the method
* that updates the scene graph and is called after collision resolution.
*
* @param dt Timing values from parent loop
*/
void TileModel::update(float dt) {
	Obstacle::update(dt);
	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawscale);
		_node->setAngle(getAngle());
	}
}
