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
    /** to keep track of how long to wait before resetting the _collisionTimeout */
    Timestamp _noLineOfSiteTimeout;
    /** to keep track of how long to wait before becoming unstunned */
    Timestamp _stunTimeout;
    /** a collision happened and we want to stop soon */
    bool _shouldStopSoon;
    /** charging or floored */
    bool _charging;
    /** milliseconds of stun */
    int _stunDuration;
    bool _waterInbetween;
protected:
	/** The scene graph node for the enemy */
	std::shared_ptr<PolygonNode> _node;
	/** The texture key for the enemy */
	std::string _texture;

	b2FrictionJoint* _frictionJoint;
	float _friction;

	/** The force to apply to this enemy */
	Vec2 _force;

	/** The ratio of the enemy sprite to the physics body */
	float _drawscale;
    
    /** timeout timer for enemy slinging */
    Timestamp _previousTime;

	bool _stunned;
	bool _onFire;

	bool _wandering;
	bool _targeting;
	
	bool _mushroom;
	bool _spore;
	bool _onion;
	bool _acorn;

	bool _destroyed;

	std::shared_ptr<cugl::AnimationNode> _sparks;
	bool _sparky;

	bool _shooting;
	bool _dispersing;

	bool _collided;
	bool _sliding;
	bool _prepping;

	int _direction;

	std::vector<Vec2> _route;

public:
	int _prepTimer;
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
#pragma mark Status
	bool isStunned() { return _stunned; }

	void setStunned(bool stunned) { _stunned = stunned; }
    
    void stunEnemy(int millis){
        _stunDuration = millis;
        _stunned = true;
        _stunTimeout.mark();
    }

	bool isFire() { return _onFire; }

	void setFire(bool fire) { _onFire = fire; }
    
    bool isWaterBetween() { return _waterInbetween; }
    
    void setWaterBetween(bool water) {
        if(_waterInbetween == false && water){
            _noLineOfSiteTimeout.mark();
            _waterInbetween = true;
        } else {
            _waterInbetween = water;
        }
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
	* Returns the current friction.
	*/
	float getFriction() { return _friction; }

	/**
	* Sets the friction of the friction joint with the ground.
	*/
	void setFriction(float friction) override { _friction = friction; _frictionJoint->SetMaxForce(friction); _frictionJoint->SetMaxTorque(friction); }

	/**
	* Sets the friction joint with the ground.
	*/
	void setFrictionJoint(b2FrictionJoint* frictionJoint) { _frictionJoint = frictionJoint; }
    
    /**
     * Sets whether enemy is charging or floored.
     *
     * @param bool for if charging .
     */
    void setCharging(bool charge) { _charging = charge; }
    
	Vec2 getPosition();

    /**
     * Sets whether enemy is charging or floored.
     *
     * @param bool for if charging .
     */
    bool getCharging() { return _charging; }

	bool isTargeting() { return _targeting; }

	void setTargeting() { _targeting = true; _wandering = false; }

	bool isWandering() { return _wandering; }

	void setWandering() { _wandering = true; _targeting = false; }

	bool isMushroom() { return _mushroom; }

	void setMushroom() { _mushroom = true; }

	bool isSpore() { return _spore; }

	void setSpore() { _spore = true; }

	bool isOnion() { return _onion; }

	void setOnion() { _onion = true; }

	bool isAcorn() { return _acorn; }

	void setAcorn() { _acorn = true; }

	bool isDestroyed() { return _destroyed; }

	void setDestroyed() { _destroyed = true; }

	std::vector<Vec2> getRoute() { return _route; }

	void setRoute(std::vector<Vec2> route) { _route = route; }

	void setSparks(const std::shared_ptr<AnimationNode>& sparks) { _sparks = sparks; }
	void setSparky(bool sparky) { _sparky = sparky; }
	bool getSparky() { return _sparky; }
	void updateSparks();
	void updateSparks(bool visible);

	bool isShooting() { return _shooting; }
	bool isDispersing() { return _dispersing; }
	void setShooting() { _shooting = true; }
	void setDispersing() { _dispersing = true; }

	void setCoalided(bool collided) { _collided = collided; }
	bool getCoalided() { return _collided; }

	void setDirection(int dir) { _direction = dir; }
	int getDirection() { return _direction; }

	void setSliding(int sliding) { _sliding = sliding; }
	int isSliding() { return _sliding; }

	void setPrepping(bool prepping) { _prepping = prepping; _prepTimer = 20; }
	bool isPrepping() { return _prepping; }

	void animateSpore();

	/**
	* Returns the scene graph node representing this enemy.
	*
	* @return the scene graph node representing this enemy.
	*/
	const std::shared_ptr<PolygonNode>& getNode() const { return _node; }

	/**
	* Sets the scene graph node representing this enemy.
	*
	* @param node  The scene graph node representing this enemy.
	*/
	void setNode(const std::shared_ptr<PolygonNode>& node) { _node = node; }

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
     * Returns true if enemy is in bounds
     */
    bool inBounds(int width, int height);
    
    /**
     * Sets the texture for enemy based on angle facing and state
     *
     * @param angle   direction enemy facing in degrees
     * @param isAcorn if the enemy is an acorn or not
     */
    void setDirectionTexture(float angle, bool isAcorn, int mode);
	void setDirectionTexture(int dir, bool isAcorn, int mode);

	/**
	* Updates the object's physics state (NOT GAME LOGIC). This is the method
	* that updates the scene graph and is called agter collision resolution.
	*
	* @param dt Timing values from parent loop
	*/
	virtual void update(float dt) override;
};

#endif /* __PLAYER_MODEL_H__ */
