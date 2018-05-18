//
//  PlayerModel.h
//  Coalide
//
#ifndef __PLAYER_MODEL_H__
#define __PLAYER_MODEL_H__
#include <cugl/cugl.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include "Constants.h"

using namespace cugl;

/**
* This class is the player avatar (Nicoal) for Coalide.
*/
class PlayerModel : public CapsuleObstacle {
private:
    /** default tint */
    Color4 _color;
    /** to keep track of how long to wait before stopping */
    Timestamp _collisionTimeout;
    /** to keep track of how long to wait before becoming unstunned */
    Timestamp _stunTimeout;
    /** a collision happened and we want to stop soon */
    bool _shouldStopSoon;
    /** charging or floored */
    bool _charging;

	bool _superCollide;
	int _superCollideTimer;
    
	/** milliseconds of stun */
    int _stunDuration;
    /** size of the player */
    Vec2 _sizePlayer;
    /** whether the player should be stunned when they stop */
    bool _stunOnStop;
    /** direction Nicoal is facing based on regions set up in constants file */
    int player_direction;
    /** Whether Nicoal got hit in mid collision */
    bool _collided;
    
    
protected:
    std::shared_ptr<AssetManager> _assets;
	std::shared_ptr<PolygonNode> _node;
    std::shared_ptr<Node> _buildupNode;
	std::shared_ptr<Node> _chargingNode;
	std::shared_ptr<Node> _arrow;
    std::shared_ptr<PolygonNode> _circle;
    /** The body animation */
    //std::shared_ptr<cugl::AnimationNode> _anim;
	std::string _texture;
    
    /** The animation actions */
    std::shared_ptr<cugl::Animate> _forward;
	std::shared_ptr<cugl::AnimationNode> _sparks;
	bool _sparky;

	Vec2 _force;
	b2FrictionJoint* _frictionJoint;
	int _friction;

	float _drawscale;

	bool _stunned;
	bool _onFire;


public:
    float _oldAngle;
    bool _isSliding;
#pragma mark Constructors
	/**
	* Creates a new player at the origin.
	*/
	PlayerModel(void) : CapsuleObstacle() { }

	/**
	* Destroys the player, releasing all resources.
	*/
	virtual ~PlayerModel(void) { dispose(); }

	/**
	* Disposes all resources and assets of this player.
	*/
	void dispose();

	/**
	* Initializes a new player with the given position and size.
	*
	* @param  pos		Initial position in world coordinates
	* @param  size   	The dimensions of the sprite.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Vec2& pos, const Size& size) override;


#pragma mark Static Constructors
	/**
	* Returns a newly allocated player with the given position and size
	*
	* @param pos   Initial position in world coordinates
	* @param size  The dimensions of the sprite.
	*
	* @return a newly allocated player with the given position
	*/
	static std::shared_ptr<PlayerModel> alloc(const Vec2& pos, const Size& size) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

#pragma mark -
#pragma mark Status
	bool isStunned() { return _stunned; }

	void setStunned() {
        if(!_stunned){
            _stunned = true;
            _stunTimeout.mark();
            _stunDuration = 3000;
            updateArrow(false);
            updateCircle(false);
        }

    }
    
    void stunOnStop(int millis){
        if(!_stunned){
            _stunTimeout.mark();
            _stunDuration = millis;
            _stunOnStop = true;
            updateArrow(false);
            updateCircle(false);
        }
    }

	bool isFire() { return _onFire; }

	void setFire(bool fire) { _onFire = fire; }
    
#pragma mark -
#pragma mark Accessors
	/**
	* Returns the force applied to thes player.
	*
	* @return the force applied to the player.
	*/
	const Vec2& getForce() const { return _force; }

	/**
	* Sets the force applied to the player.
	*
	* @param value the force applied to the player.
	*/
	void setForce(const Vec2& value) { _force.set(value); }
    
	/**
	* Returns the current friction.
	*/
	int getFriction() { return _friction; }

	/**
	* Sets the friction of the friction joint with the ground.
	*/
	void setFriction(int friction) { _friction = friction; _frictionJoint->SetMaxForce(friction); _frictionJoint->SetMaxTorque(friction); }

	/**
	* Sets the friction joint with the ground.
	*/
	void setFrictionJoint(b2FrictionJoint* frictionJoint) { _frictionJoint = frictionJoint; }
    
    /**
     * Sets whether player is charging or floored.
     *
     * @param bool for if charging .
     */
    void setCharging(bool charge) { _charging = charge; }
    
    /**
    * Sets whether player is charging or floored.
    *
    * @param bool for if charging .
    */
    bool getCharging() { return _charging; }

	/**
	* Returns the scene graph node representing the player.
	*
	* @return the scene graph node representing the player.
	*/
	const std::shared_ptr<PolygonNode>& getNode() const { return _node; }

	/**
	* Sets the scene graph node representing the player.
	*
	* @param node  The scene graph node representing the player.
	*/
	void setNode(const std::shared_ptr<PolygonNode>& node) { _node = node; }

	/**
	* Returns the scene graph node representing the player.
	*
	* @return the scene graph node representing the player.
	*/
	const std::shared_ptr<Node>& getArrow() const { return _arrow; }
    const std::shared_ptr<PolygonNode>& getCircle() const { return _circle; }

	/**
	* Sets the scene graph node representing the player.
	*
	* @param node  The scene graph node representing the player.
	*/
	void setArrow(const std::shared_ptr<Node>& arrow) { _arrow = arrow; }
    void setCircle(const std::shared_ptr<PolygonNode>& circle) { _circle = circle; }
	void setSparks(const std::shared_ptr<AnimationNode>& sparks) { _sparks = sparks; }
    
    void setDirectionTexture(float angle, int mode);
    void setDirectionTexture(int dir, int mode);
    
    void setPlayerDirection(int dir) { player_direction = dir; }
    int getPlayerDirection() {return player_direction; }
    
    void setCoalided(bool collided) { _collided = collided; }
    bool getCoalided() {return _collided; }

	void setSparky(bool sparky) { _sparky = sparky; }
	bool getSparky() { return _sparky; }

	/**
	* Returns the texture (key) for the player.
	*
	* @return the texture (key) for the player.
	*/
	const std::string& getTextureKey() const { return _texture; }

	/**
	* Returns the texture (key) for the player.
	*
	* @param strip the texture (key) for the player.
	*/
	void setTextureKey(const std::string& strip) { _texture = strip; }

	/**
	* Returns the ratio of the player sprite to the physics body.
	*
	* @return the ratio of the player sprite to the physics body.
	*/
	float getDrawScale() const { return _drawscale; }

	/**
	* Sets the ratio of the player sprite to the physics body.
	*
	* @param scale The ratio of the player sprite to the physics body.
	*/
	void setDrawScale(float scale) { _drawscale = scale; }
    
    /**
     * Set the default tint of the player model
     */
    void setColor(Color4 c){
        _color = c;
    }

	Vec2 getPosition();
    
	void setSuperCollide(bool superCollide) {
		_superCollideTimer = SUPER_COLLISION_LENGTH;
		_superCollide = superCollide;
	}

	bool isSuperCollide();

    /**
     * Is this player already stopping soon
     */
    bool alreadyStopping() {
        return _shouldStopSoon;
    }

	/**
	* Updates the aim arrow.
	*
	* @param node  updates the aim arrow.
	*/
    void updateArrow(cugl::Vec2 aim, std::shared_ptr<Node> currNode, bool visible);
	void updateArrow(bool visible);
    void updateCircle(cugl::Vec2 aim, std::shared_ptr<Node> currNode, bool visible);
    void updateCircle(bool visible);
	void updateSparks();
	void updateSparks(bool visible);

#pragma mark -
#pragma mark Physics
	/**
	* Applies the force to the body of this player
	*/
    void applyLinearImpulse(Vec2& impulse);
    
    /**
     * Stop this player after timeout milliseconds
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
    
#pragma mark -
#pragma mark Logic
    /**
     * Returns true if the player is moving slow enough to sling
     */
    bool canSling();
    
    /**
     * Returns true if player is in bounds
     */
    bool inBounds(int width, int height);
    
};

#endif /* __PLAYER_MODEL_H__ */
