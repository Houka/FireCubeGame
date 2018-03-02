//
//  PlayerModel.h
//  Coalide
//
#ifndef __PLAYER_MODEL_H__
#define __PLAYER_H__
#include <cugl/cugl.h>

using namespace cugl;


/**
* This class is the player avatar (Nicoal) for Coalide.
*/
class PlayerModel : public CapsuleObstacle {
protected:
	std::shared_ptr<Node> _node;
	std::string _texture;

	Vec2 _force;

	float _drawscale;

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


#pragma mark -
#pragma mark Physics
	/**
	* Applies the force to the body of this player
	*/
	void applyForce();

	/**
	* Updates the object's physics state (NOT GAME LOGIC). This is the method 
	* that updates the scene graph and is called agter collision resolution.
	*
	* @param dt Timing values from parent loop
	*/
	virtual void update(float dt) override;
};

#endif /* __PLAYER_MODEL_H__ */
