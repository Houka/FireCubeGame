//
//  TileMode.h
//  Coalide
//
#ifndef __TILE_MODEL_H__
#define __TILE_MODEL_H__

#include <cugl/2d/physics/CUBoxObstacle.h>
#include "Constants.h"

using namespace cugl;


#pragma mark -
#pragma mark Tile Model

/**
* A crate in the rocket lander game.
*
* Note that this class does keeps track of its own assets via keys, so that
* they can be attached later as part of the scene graph.
*/
class TileModel : public BoxObstacle {
protected:
	std::shared_ptr<Node> _node;
	std::string _tileTexture;
	TILE_TYPE _tileType;
	float _friction;
	
	float _drawscale;


public:
#pragma mark -
#pragma mark Constructors
	/*
	* Creates a new floor tile at the origin.
	*/
	TileModel() : BoxObstacle() { }

	/**
	* Destroys this floor tile, releasing all resources.
	*/
	virtual ~TileModel() { dispose(); }

	/**
	* Destroys this floor tile, releasing all resources.
	*/
	void dispose();

	/**
	* Initializes a new floor tile at the origin.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init() override { return init(Vec2::ZERO, Size::ZERO); }

	/**
	* Initializes a new floor tile with the given position
	*
	* @param  pos  Initial position in world coordinates
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Vec2& pos) override { return init(pos, Size(1, 1)); }

	/**
	* Initializes a new floor tile with the given position and size.
	*
	* @param  pos      Initial position in world coordinates
	* @param  size   	The dimensions of the crate.
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Vec2& pos, const Size& size) override;


#pragma mark -
#pragma mark Static Constructors
	/**
	* Creates a new floor tile at the origin.
	*
	* @return  A newly allocated Crate, at the origin
	*/
	static std::shared_ptr<TileModel> alloc() {
		std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	* Creates a new floor tile with the given position
	*
	* @param  pos  Initial position in world coordinates
	*
	* @return  A newly allocated Crate, at the given position
	*/
	static std::shared_ptr<TileModel> alloc(const Vec2& pos) {
		std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
		return (result->init(pos) ? result : nullptr);
	}

	/**
	* Creates a new floor tile with the given position and size.
	*
	* @param  pos      Initial position in world coordinates
	* @param  size   	The dimensions of the crate.
	*
	* @return  A newly allocated Crate, at the given position, with the given size
	*/
	static std::shared_ptr<TileModel> alloc(const Vec2& pos, const Size& size) {
		std::shared_ptr<TileModel> result = std::make_shared<TileModel>();
		return (result->init(pos, size) ? result : nullptr);
	}


#pragma mark -
#pragma mark Accessors
	/**
	* Sets the tile type.
	*/
	void setType(TILE_TYPE tileType) { _tileType = tileType; }

	/**
	* Returns the friction.
	*/
	float getFriction() { return _friction; }

	/**
	* Sets the friction.
	*/
	void setFriction(float friction) { _friction = friction; }

	/**
	* Returns the scene graph node representing this floor tile.
	*
	* @return the scene graph node representing this floor tile.
	*/
	const std::shared_ptr<Node>& getNode() const { return _node; }

	/**
	* Sets the scene graph node representing this floor tile.
	*
	* @param node  The scene graph node representing this floor tile.
	*/
	void setNode(const std::shared_ptr<Node>& node) { _node = node; }

	/**
	* Returns the texture (key) for this floor tile
	*
	* @return the texture (key) for this floor tile
	*/
	const std::string& getTextureKey() const { return _tileTexture; }

	/**
	* Sets the texture (key) for this floor tile
	*
	* @param  strip    the texture (key) for this floor tile
	*/
	void setTextureKey(std::string strip) { _tileTexture = strip; }

	/**
	* Returns the ratio of the sprite to the physics body.
	*
	* @return the ratio of the sprite to the physics body.
	*/
	float getDrawScale() const { return _drawscale; }

	/**
	* Sets the ratio of the sprite to the physics body.
	*
	* @param scale The ratio of the sprite to the physics body.
	*/
	void setDrawScale(float scale) { _drawscale = scale; }

#pragma mark -
#pragma mark Type Checkers
	/**
	* Returns true if the tile is water
	*/
	bool isWater() {
		return _tileType == TILE_TYPE::WATER;
	}

#pragma mark -
#pragma mark Update
	/**
	* Updates the object's physics state (NOT GAME LOGIC). This is the method
	* that updates the scene graph and is called after collision resolution.
	*
	* @param dt Timing values from parent loop
	*/
	void update(float dt);
};
#endif /* __TILE_MODEL_H__ */
