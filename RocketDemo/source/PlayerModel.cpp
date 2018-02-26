//
//  JSRocketModel.cpp
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
#include "PlayerModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

/** The number of frames for the afterburner */
#define FIRE_FRAMES     4

#pragma mark -
#pragma mark Constructors

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
bool PlayerModel::init(const Vec2& pos, const Size& size) {
	BoxObstacle::init(pos, size);
	std::string name("player");
	setName(name);

	_shipNode = nullptr;

	return true;
}


/**
* Disposes all resources and assets of this rocket
*
* Any assets owned by this object will be immediately released.  Once
* disposed, a rocket may not be used until it is initialized again.
*/
void PlayerModel::dispose() {
	_shipNode = nullptr;
	_shipTexture = "";
}


#pragma mark -
#pragma mark Physics
/**
* Applies the force to the body of this ship
*
* This method should be called after the force attribute is set.
*/
void PlayerModel::applyForce() {
	if (!isActive()) {
		return;
	}
	// Orient the force with rotation.
	Vec4 netforce(_force.x, _force.y, 0.0f, 1.0f);
	Mat4::createRotationZ(getAngle(), &_affine);
	netforce *= _affine;

	// Apply force to the rocket BODY, not the rocket
	_body->ApplyForceToCenter(b2Vec2(netforce.x, netforce.y), true);
}

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
void PlayerModel::update(float delta) {
	//CULog("Updating.");
	Obstacle::update(delta);
	if (_shipNode != nullptr) {
		CULog(getPosition().toString().c_str());
		_shipNode->setPosition(getPosition());
		_shipNode->setAngle(getAngle());
	}
}



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
void PlayerModel::setDrawScale(float scale) {
	_drawscale = scale;
	if (_shipNode != nullptr) {
		_shipNode->setPosition(getPosition()*_drawscale);
	}
}

