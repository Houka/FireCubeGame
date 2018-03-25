//
//  PlayerModel.h
//  Coalide
//
#ifndef __PLAYER_MODEL_H__
#define __PLAYER_MODEL_H__
#include <cugl/cugl.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>

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
    /** a collision happened and we want to stop soon */
    bool _shouldStopSoon;
protected:
	std::shared_ptr<Node> _node;
	std::shared_ptr<PathNode> _arrow;
	std::string _texture;

	Vec2 _force;
	b2FrictionJoint* _frictionJoint;
	float _friction;

	float _drawscale;

	bool _stunned;
	bool _onFire;

	int _stunTimer;

public:
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

	void setStunned() { _stunned = true; _stunTimer = 0; }

	void stillStunned();

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
	* Returns the scene graph node representing the player.
	*
	* @return the scene graph node representing the player.
	*/
	const std::shared_ptr<Node>& getNode() const { return _node; }

	/**
	* Sets the scene graph node representing the player.
	*
	* @param node  The scene graph node representing the player.
	*/
	void setNode(const std::shared_ptr<Node>& node) { _node = node; }

	/**
	* Returns the scene graph node representing the player.
	*
	* @return the scene graph node representing the player.
	*/
	const std::shared_ptr<PathNode>& getArrow() const { return _arrow; }

	/**
	* Sets the scene graph node representing the player.
	*
	* @param node  The scene graph node representing the player.
	*/
	void setArrow(const std::shared_ptr<PathNode>& arrow) { _arrow = arrow; }

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
	void updateArrow(cugl::Vec2 aim, bool visible);
	void updateArrow(bool visible);

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
    
};

#endif /* __PLAYER_MODEL_H__ */
