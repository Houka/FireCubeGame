//
//	ObjectModel.cpp
//	Coalide
//

#include "ObjectModel.h"
#include "Constants.h"
#include <cugl/util/CUTimestamp.h>

#define MAX_SPEED_FOR_SLING 2
#define IMPULSE_SCALE 8
#define SLING_TIMEOUT 4000
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
bool ObjectModel::init(const Vec2 & pos, const Size & size) {
	if (CapsuleObstacle::init(pos, size)) {
		// defaults
		setName(INANIMATE_NAME);
		setBodyType(b2_staticBody);

		_node = nullptr;

		setDensity(4.0f);
		setRestitution(0.4f);
		setFixedRotation(true);

		_broken = false;
		_animating = false;

		return true;
	}
	return false;
}

void ObjectModel::dispose() {
	_node = nullptr;
	_frictionJoint = nullptr;
}

void ObjectModel::animate() {
	std::shared_ptr<AnimationNode> node = std::dynamic_pointer_cast<AnimationNode>(_node);
	int frame = node->getFrame();
	if (isBreakable()) {
		if (frame < 5) {
			node->setFrame(frame + 1);
		}
		else {
			setBroken();
		}
	}
	else if (frame < 11) {
		node->setFrame(frame + 1);
	}
}


Vec2 ObjectModel::getPosition() {
	return Vec2(CapsuleObstacle::getPosition().x, CapsuleObstacle::getPosition().y + 0.5);
}


/**
* Updates the object's physics state (NOT GAME LOGIC). This is the method
* that updates the scene graph and is called agter collision resolution.
*
* @param dt Timing values from parent loop
*/
void ObjectModel::update(float dt) {
	Obstacle::update(dt);
	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawscale);
		_node->setAngle(getAngle());
	}

	if (_shouldStopSoon && Timestamp().ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT) {
		_shouldStopSoon = false;
		_body->SetLinearVelocity(b2Vec2(0, 0));
	}
}
