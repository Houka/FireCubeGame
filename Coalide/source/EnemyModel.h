//
//	EnemyModel.h
//	Coalide
//
#ifndef __ENEMY_MODEL_H__
#define __ENEMY_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/util/CUTimestamp.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>

using namespace cugl;

/**
* This class is the enemy avatar.
*/
class EnemyModel : public CapsuleObstacle {
private:
    /** random reduction for the timer between slings */
    int _rndTimerReduction;
    /** to keep track of how long to wait before stopping */
    Timestamp _collisionTimeout;
    /** a collision happened and we want to stop soon */
    bool _shouldStopSoon;
protected:
	/** The scene graph node for the enemy */
	std::shared_ptr<Node> _node;
	/** The texture key for the enemy */
	std::string _texture;

	b2FrictionJoint* _frictionJoint;

	/** The force to apply to this enemy */
	Vec2 _force;

	/** The ratio of the enemy sprite to the physics body */
	float _drawscale;
    
    /** timeout timer for enemy slinging */
    Timestamp _previousTime;

public:
#pragma mark Constructors
	/**
	* Creates a new enemy at the origin.
	*/
	EnemyModel(void) : CapsuleObstacle() { }

	/**
	* Destroys this enemy, releasing all resources.
	*/
	virtual ~EnemyModel(void) { dispose(); }

	/**
	* Disposes all resources and assets of this enemy.
	*/
	void dispose();

	/**
	* Initializes a new enemy with the given position and size.
	*
	* @param  pos		Initial position in world coordinates
	* @param  size   	The dimensions of the sprite.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Vec2& pos, const Size& size) override;


#pragma mark Static Constructors
	/**
	* Returns a newly allocated enemy with the given position and size
	*
	* @param pos   Initial position in world coordinates
	* @param size  The dimensions of the sprite.
	*
	* @return a newly allocated enemy with the given position
	*/
	static std::shared_ptr<EnemyModel> alloc(const Vec2& pos, const Size& size) {
		std::shared_ptr<EnemyModel> result = std::make_shared<EnemyModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

#pragma mark -
#pragma mark Accessors
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
	* Sets the friction of the friction joint with the ground.
	*/
	void setFriction(float friction) { _frictionJoint->SetMaxForce(friction); _frictionJoint->SetMaxTorque(friction); }

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
#pragma mark Logic
    /**
     * Returns true if the enemy is moving slow enough to sling
     */
    bool canSling();
    
    /**
     * Returns true if the enough time has elapsed since the last sling
     */
    bool timeoutElapsed();

#pragma mark -
#pragma mark Physics
    /**
     * Applies the force to the body of this enemy
     */
    void applyLinearImpulse(Vec2& impulse);
    
    /**
     * Stop this enemy after timeout milliseconds
     */
    void setShouldStop(){
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

#endif /* __PLAYER_MODEL_H__ */
