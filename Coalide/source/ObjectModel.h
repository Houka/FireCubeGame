//
//	ObjectModel.h
//	Coalide
//
#ifndef __OBJECT_MODEL_H__
#define __OBJECT_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/util/CUTimestamp.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include "Constants.h"

using namespace cugl;

/**
* This class is the inanimate object.
*/
class ObjectModel : public CapsuleObstacle {
protected:
	/** The scene graph node */
	std::shared_ptr<Node> _node;
	/** The texture key */
	std::string _texture;

	/** to keep track of how long to wait before stopping */
	Timestamp _collisionTimeout;
	/** a collision happened and we want to stop soon */
	bool _shouldStopSoon;

	b2FrictionJoint* _frictionJoint;
	float _friction;

	/** The force to apply */
	Vec2 _force;

	bool _isBroken;

	/** The ratio of the sprite to the physics body */
	float _drawscale;

	/** The type of object */
	OBJECT_TYPE _type;

public:
#pragma mark Constructors
	/**
	* Creates a new enemy at the origin.
	*/
	ObjectModel(void) : CapsuleObstacle() { }

	/**
	* Destroys this enemy, releasing all resources.
	*/
	virtual ~ObjectModel(void) { dispose(); }

	/**
	* Disposes all resources and assets.
	*/
	void dispose();

	/**
	* Initializes a new object with the given position and size.
	*
	* @param  pos		Initial position in world coordinates
	* @param  size   	The dimensions of the sprite.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Vec2& pos, const Size& size) override;


#pragma mark Static Constructors
	/**
	* Returns a newly allocated object with the given position and size
	*
	* @param pos   Initial position in world coordinates
	* @param size  The dimensions of the sprite.
	*
	* @return a newly allocated enemy with the given position
	*/
	static std::shared_ptr<ObjectModel> alloc(const Vec2& pos, const Size& size) {
		std::shared_ptr<ObjectModel> result = std::make_shared<ObjectModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

#pragma mark -
#pragma mark Accessors
	/**
	* Sets the object type.
	*/
	void setType(OBJECT_TYPE type) { _type = type; }

	/** Returns true if the object breaks after one hit */
	bool isBreakable() { return _type == OBJECT_TYPE::BREAKABLE; }

	/** Returns true if the object move on being hit */
	bool isMovable() { return _type == OBJECT_TYPE::MOVABLE; }

	/** Returns true if the object cannot be moved */
	bool isImmobile() { return _type == OBJECT_TYPE::IMMOBILE; }

	/** Returns true if an object has been hit already and been broken */
	bool isBroken() { return _isBroken; }

	/** Sets the brokenness state of the object */
	void setBroken(bool broken) { _isBroken = broken; }

	/**
	* Returns the force applied to this enemy.
	*
	* @return the force applied to this enemy.
	*/
	const Vec2& getForce() const { return _force; }

	/**
	* Sets the force applied to this enemy.
	*
	* @param value the force applied to this enemy.
	*/
	void setForce(const Vec2& value) { _force.set(value); }

	/**
	* Returns the current friction.
	*/
	float getFriction() { return _friction; }

	/**
	* Sets the friction of the friction joint with the ground.
	*/
	void setFriction(float friction) { _friction = friction; _frictionJoint->SetMaxForce(friction); _frictionJoint->SetMaxTorque(friction); }

	/**
	* Sets the friction joint with the ground.
	*/
	void setFrictionJoint(b2FrictionJoint* frictionJoint) { _frictionJoint = frictionJoint; }

	/**
	* Returns the scene graph node representing this enemy.
	*
	* @return the scene graph node representing this enemy.
	*/
	const std::shared_ptr<Node>& getNode() const { return _node; }

	/**
	* Sets the scene graph node representing this enemy.
	*
	* @param node  The scene graph node representing this enemy.
	*/
	void setNode(const std::shared_ptr<Node>& node) { _node = node; }

	/**
	* Returns the texture (key) for this enemy.
	*
	* @return the texture (key) for this enemy.
	*/
	const std::string& getTextureKey() const { return _texture; }

	/**
	* Returns the texture (key) for this enemy.
	*
	* @param strip the texture (key) for this enemy.
	*/
	void setTextureKey(const std::string& strip) { _texture = strip; }

	/**
	* Returns the ratio of the enemy sprite to the physics body.
	*
	* @return the ratio of the enemy sprite to the physics body.
	*/
	float getDrawScale() const { return _drawscale; }

	/**
	* Sets the ratio of the enemy sprite to the physics body.
	*
	* @param scale The ratio of the enemy sprite to the physics body.
	*/
	void setDrawScale(float scale) { _drawscale = scale; }

	/**
	* Is this enemy already stopping soon
	*/
	bool alreadyStopping() {
		return _shouldStopSoon;
	}

#pragma mark -
#pragma mark Physics
	/**
	* Stop this enemy after timeout milliseconds
	*/
	void setShouldStop() {
		_collisionTimeout.mark();
		_shouldStopSoon = true;
	}

	/**
	* Updates the object's physics state (NOT GAME LOGIC). This is the method
	* that updates the scene graph and is called agter collision resolution.
	*
	* @param dt Timing values from parent loop
	*/
	virtual void update(float dt) override;
};

#endif /* __OBJECT_MODEL_H__ */
