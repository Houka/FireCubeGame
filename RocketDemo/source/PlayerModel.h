//
//  JSRocketModel.h
//  JSON Demo
//
//  This encapsulates all of the information for the rocket demo.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Pay close attention to how this class designed. This class uses our standard
//  shared-pointer architecture which is common to the entire engine.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Note that this object manages its own texture, but DOES NOT manage its own
//  scene graph node.  This is a very common way that we will approach complex
//  objects.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version: 3/12/17
//
#ifndef __PLAYER_MODEL_H__
#define __PLAYER_H__
#include <cugl/cugl.h>

/**
* This class is the player avatar for the rocket lander game.
*
* There are two ways to link a physics object to a scene graph node on the
* screen.  One way is with callback functions, like we did with the crates.
* The other way is to create a subclass of a physics object and have it track
* its own scene graph node.  This class is an example of the latter.
*/
class PlayerModel : public cugl::BoxObstacle {
private:
	/** This macro disables the copy constructor (not allowed on scene graphs) */
	CU_DISALLOW_COPY_AND_ASSIGN(PlayerModel);

protected:
	/** The force to apply to this rocket */
	cugl::Vec2 _force;
	/** The thrust of the rocket */
	float _thrust;

	/** The scene graph node for the rocket ship */
	std::shared_ptr<cugl::Node> _shipNode;
	/** The texture key for the ship */
	std::string _shipTexture;

	/** Cache object for transforming the force according the object angle */
	cugl::Mat4 _affine;
	float _drawscale;

public:
#pragma mark Constructors
	/**
	* Creates a new rocket at the origin.
	*
	* NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	* the heap, use one of the static constructors instead.
	*/
	PlayerModel(void) : BoxObstacle() { }

	/**
	* Destroys this rocket, releasing all resources.
	*/
	virtual ~PlayerModel(void) { dispose(); }

	/**
	* Disposes all resources and assets of this rocket
	*
	* Any assets owned by this object will be immediately released.  Once
	* disposed, a rocket may not be used until it is initialized again.
	*/
	void dispose();

	/**
	* Initializes a new rocket at the origin.
	*
	* The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
	* 1 pixel = 1 Box2d unit
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init() override { return init(cugl::Vec2::ZERO, cugl::Size::ZERO); }

	/**
	* Initializes a new rocket with the given position
	*
	* The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
	* 1 pixel = 1 Box2d unit
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @param  pos  Initial position in world coordinates
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const cugl::Vec2& pos) override { return init(pos, cugl::Size(1, 1)); }

	/**
	* Initializes a new rocket with the given position and size.
	*
	* The rocket size is specified in world coordinates.
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @param  pos      Initial position in world coordinates
	* @param  size   	The dimensions of the box.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const cugl::Vec2& pos, const cugl::Size& size) override;


#pragma mark Static Constructors
	/**
	* Returns a newly allocate rocket at the origin.
	*
	* The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
	* 1 pixel = 1 Box2d unit
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @return a newly allocate rocket at the origin.
	*/
	static std::shared_ptr<PlayerModel> alloc() {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	* Returns a newly allocated rocket with the given position
	*
	* The rocket is 1 unit by 1 unit in size. The rocket is scaled so that
	* 1 pixel = 1 Box2d unit
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @param pos   Initial position in world coordinates
	*
	* @return a newly allocated rocket with the given position
	*/
	static std::shared_ptr<PlayerModel> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos) ? result : nullptr);
	}

	/**
	* Returns a newly allocated rocket with the given position and size
	*
	* The rocket size is specified in world coordinates.
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @param pos   Initial position in world coordinates
	* @param size  The dimensions of the box.
	*
	* @return a newly allocated rocket with the given position
	*/
	static std::shared_ptr<PlayerModel> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<PlayerModel> result = std::make_shared<PlayerModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

#pragma mark -
#pragma mark Accessors
	/**
	* Returns the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the force applied to this rocket.
	*/
	const cugl::Vec2& getForce() const { return _force; }

	/**
	* Sets the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value  the force applied to this rocket.
	*/
	void setForce(const cugl::Vec2& value) { _force.set(value); }

	/**
	* Sets thethrust of this rocket.
	*
	* @param value  the thrust of this rocket
	*/
	void setThrust(float value) { _thrust = value; }

	/**
	* Returns the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the x-component of the force applied to this rocket.
	*/
	float getFX() const { return _force.x; }

	/**
	* Sets the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value the x-component of the force applied to this rocket.
	*/
	void setFX(float value) { _force.x = value; }

	/**
	* Returns the y-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the y-component of the force applied to this rocket.
	*/
	float getFY() const { return _force.y; }

	/**
	* Sets the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value the x-component of the force applied to this rocket.
	*/
	void setFY(float value) { _force.y = value; }

	/**
	* Returns the amount of thrust that this rocket has.
	*
	* Multiply this value times the horizontal and vertical values in the
	* input controller to get the force.
	*
	* @return the amount of thrust that this rocket has.
	*/
	float getThrust() const { return _thrust; }


#pragma mark -
#pragma mark Animation
	/**
	* Returns the scene graph node representing this rocket.
	*
	* By storing a reference to the scene graph node, the model can update
	* the node to be in sync with the physics info. It does this via the
	* {@link Obstacle#update(float)} method.
	*
	* @return the scene graph node representing this rocket.
	*/
	const std::shared_ptr<cugl::Node>& getShipNode() const { return _shipNode; }

	/**
	* Returns the texture (key) for this ship
	*
	* The value returned is not a Texture2D value.  Instead, it is a key for
	* accessing the texture from the asset manager.
	*
	* @return the texture (key) for this ship
	*/
	const std::string& getTextureKey() const { return _shipTexture; }

	/**
	* Returns the texture (key) for this ship
	*
	* The value returned is not a Texture2D value.  Instead, it is a key for
	* accessing the texture from the asset manager.
	*
	* @param  strip    the texture (key) for this ship
	*/
	void setTextureKey(const std::string& strip) { _shipTexture = strip; }

	/**
	* Sets the ratio of the ship sprite to the physics body
	*
	* The rocket needs this value to convert correctly between the physics
	* coordinates and the drawing screen coordinates.  Otherwise it will
	* interpret one Box2D unit as one pixel.
	*
	* All physics scaling must be uniform.  Rotation does weird things when
	* attempting to scale physics by a non-uniform factor.
	*
	* @param scale The ratio of the ship sprite to the physics body
	*/
	void setDrawScale(float scale);

	/**
	* Returns the ratio of the ship sprite to the physics body
	*
	* The rocket needs this value to convert correctly between the physics
	* coordinates and the drawing screen coordinates.  Otherwise it will
	* interpret one Box2D unit as one pixel.
	*
	* All physics scaling must be uniform.  Rotation does weird things when
	* attempting to scale physics by a non-uniform factor.
	*
	* @return the ratio of the ship sprite to the physics body
	*/
	float getDrawScale() const { return _drawscale; }


#pragma mark -
#pragma mark Physics
	/**
	* Applies the force to the body of this ship
	*
	* This method should be called after the force attribute is set.
	*/
	void applyForce();

	/**
	* Updates the object's physics state (NOT GAME LOGIC).
	*
	* This method is called AFTER the collision resolution state. Therefore, it
	* should not be used to process actions or any other gameplay information.
	* Its primary purpose is to adjust changes to the fixture, which have to
	* take place after collision.
	*
	* In other words, this is the method that updates the scene graph.  If you
	* forget to call it, it will not draw your changes.
	*
	* @param delta Timing values from parent loop
	*/
	virtual void update(float delta) override;

	void setShipNode(const std::shared_ptr<cugl::Node>& node) { _shipNode = node; }
};

#endif /* __PLAYER_MODEL_H__ */
