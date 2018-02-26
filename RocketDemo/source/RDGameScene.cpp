//
//  RDGameMode.h
//  Rocket Demo
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It also handles collision detection. There is not
//  much to do for collisions; our ObstacleWorld class takes care of all
//  of that for us.  This controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "RDGameScene.h"
#include "LevelConstants.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "PlayerModel.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

using namespace cugl;


#pragma mark -
#pragma mark Level Geography

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -4.9f

/** Color to outline the physics nodes */
#define STATIC_COLOR    Color4::YELLOW
/** Opacity of the physics outlines */
#define DYNAMIC_COLOR   Color4::GREEN

/** The key for collisions sounds */
#define COLLISION_SOUND     "bump"
/** The key for the main afterburner sound */
#define MAIN_FIRE_SOUND     "burn"
/** The key for the right afterburner sound */
#define RGHT_FIRE_SOUND     "right-burn"
/** The key for the left afterburner sound */
#define LEFT_FIRE_SOUND     "left-burn"

/** The key for the font reference */
#define PRIMARY_FONT        "retro"

#pragma mark Physics Constants

// Physics constants for initialization
/** Density of non-crate objects */
#define BASIC_DENSITY       0.0f
/** Density of the crate objects */
#define CRATE_DENSITY       1.0f
/** Friction of non-crate objects */
#define BASIC_FRICTION      0.1f
/** Friction of the crate objects */
#define CRATE_FRICTION      0.2f
/** Angular damping of the crate objects */
#define CRATE_DAMPING       1.0f
/** Collision restitution for all objects */
#define BASIC_RESTITUTION   0.1f
/** Threshold for generating sound on collision */
#define SOUND_THRESHOLD     3


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : Scene(),
_complete(false),
_debug(false)
{    
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 * @param gravity   The gravitational force on this Box2d world
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }

	_gamestate = assets->get<GameState>(PROTO_LEVEL_KEY);
	if (_gamestate == nullptr) {
		CULog("Fail!");
		return false;
	}
    
    // Start up the input handler
    _assets = assets;
    _input.init();
    
    // Create the world and attach the listeners.
	std::shared_ptr<ObstacleWorld> physicsWorld = _gamestate->getPhysicsWorld();
	activateWorldCollisions(physicsWorld);
    
    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    _scale = dimen.width/physicsWorld->getBounds().getMaxX();
    
    // Create the scene graph
	_rootnode = Node::alloc();
	_rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_rootnode->setPosition(Vec2::ZERO);

	_winnode = Label::alloc("VICTORY!", _assets->get<Font>(PRIMARY_FONT));
	_winnode->setPosition(dimen / 2.0f);
	_winnode->setForeground(STATIC_COLOR);
	_winnode->setVisible(false);

	_loadnode = Label::alloc("RESET", _assets->get<Font>(PRIMARY_FONT));
	_loadnode->setPosition(dimen / 2.0f);
	_loadnode->setForeground(STATIC_COLOR);
	_loadnode->setVisible(false);

	addChild(_rootnode, 0);
	addChild(_winnode, 3);
	addChild(_loadnode, 4);

	_rootnode->setContentSize(dimen);
	_gamestate->setAssets(_assets);
	_gamestate->setRootNode(_rootnode); // Obtains ownership of root.
    
    _active = true;
    _complete = false;
    //setDebug(false);
    
    // XNA nostalgia
    Application::get()->setClearColor(Color4f::CORNFLOWER);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
		_input.dispose();
		_rootnode = nullptr;
		_winnode = nullptr;
		_loadnode = nullptr;
		_gamestate = nullptr;
		_complete = false;
		_debug = false;
		Scene::dispose();
    }
}


#pragma mark -
#pragma mark Physics Handling

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void GameScene::update(float dt) {
	if (_gamestate == nullptr) {
		return;
	}

	// Check to see if new level loaded yet
	if (_loadnode->isVisible()) {
		if (_assets->complete()) {
			_gamestate = nullptr;

			// Access and initialize level
			_gamestate = _assets->get<GameState>(PROTO_LEVEL_KEY);
			_gamestate->setAssets(_assets);
			_gamestate->setRootNode(_rootnode); // Obtains ownership of root.
			//_gamestate->showDebug(_debug);

			activateWorldCollisions(_gamestate->getPhysicsWorld());

			_loadnode->setVisible(false);
		}
		else {
			// Level is not loaded yet; refuse input
			return;
		}
	}
	_input.update(dt);

	// Process the toggled key commands
	// if (_input.didDebug()) { setDebug(!isDebug()); }
	if (_input.didReset()) {
		// Unload the level but keep in memory temporarily
		_assets->unload<GameState>(PROTO_LEVEL_KEY);

		// Load a new level and quit update
		_loadnode->setVisible(true);
		_assets->load<GameState>(PROTO_LEVEL_KEY, PROTO_LEVEL_FILE);
		setComplete(false);
		return;
	}
	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

	// Apply the force to the rocket
	std::shared_ptr<PlayerModel> player = _gamestate->getPlayer();
	player->setFX(_input.getHorizontal() * player->getThrust());
	player->setFY(_input.getVertical() * player->getThrust());
	player->applyForce();

	// Animate the three burners
	/*updateBurner(RocketModel::Burner::MAIN, rocket->getFY() >  1);
	updateBurner(RocketModel::Burner::LEFT, rocket->getFX() >  1);
	updateBurner(RocketModel::Burner::RIGHT, rocket->getFX() <  -1);*/

	// Turn the physics engine crank.
	_gamestate->getPhysicsWorld()->update(dt);
}

void GameScene::activateWorldCollisions(const std::shared_ptr<ObstacleWorld>& physicsWorld) {
	physicsWorld->activateCollisionCallbacks(true);
	physicsWorld->onBeginContact = [this](b2Contact* contact) {
		beginContact(contact);
	};
	physicsWorld->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
		beforeSolve(contact, oldManifold);
	};
}

/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void GameScene::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
}

/**
 * Handles any modifications necessary before collision resolution
 *
 * This method is called just before Box2D resolves a collision.  We use this method
 * to implement sound on contact, using the algorithms outlined in Ian Parberry's
 * "Introduction to Game Physics with Box2D".
 *
 * @param  contact  	The two bodies that collided
 * @param  oldManfold  	The collision manifold before contact
 */
void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	CULog("Colliding!");
    float speed = 0;

    // Use Ian Parberry's method to compute a speed threshold
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    for(int ii =0; ii < 2; ii++) {
        if (state2[ii] == b2_addState) {
            b2Vec2 wp = worldManifold.points[0];
            b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 dv = v1-v2;
            speed = b2Dot(dv,worldManifold.normal);
        }
    }
    
    // Play a sound if above threshold
    if (speed > SOUND_THRESHOLD) {
        // These keys result in a low number of sounds.  Too many == distortion.
        std::string key = ((Obstacle*)body1->GetUserData())->getName()+((Obstacle*)body2->GetUserData())->getName();
        auto source = _assets->get<Sound>(COLLISION_SOUND);
        if (!AudioEngine::get()->isActiveEffect(key)) {
            AudioEngine::get()->playEffect(key, source, false, source->getVolume());
        }
    }
}

