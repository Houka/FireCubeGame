//
//  GameScene.cpp
//  Coalide
//
#include "GameScene.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "Constants.h"
#include "LevelController.h"
#include "PlayerModel.h"

#include <string>

using namespace cugl;


#pragma mark -
#pragma mark Constructors
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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, InputController input) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= GAME_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene::init(dimen)) {
		return false;
	}

	// assets and input come from the CoalideApp level
	_assets = assets;
	_input = input;
	
	// Initialize the controllers used in the game mode
	_collisions.init();
	_ai.init();
    _input.init();


	// Get the loaded level
	_gamestate = assets->get<LevelController>(PROTO_LEVEL_KEY)->getGameState();
	
	if (_gamestate == nullptr) {
		CULog("Fail!");
		return false;
	}

	// Activate the collision callbacks for the physics world
	activateWorldCollisions();

	// Set up the scene graph
	createSceneGraph(dimen);

	return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void GameScene::dispose() {
	if (_active) {
		_input.dispose();
		_collisions.dispose();
		_ai.dispose();
		_rootnode = nullptr;
		_winnode = nullptr;
		_losenode = nullptr;
		_loadnode = nullptr;
		_gamestate = nullptr;
		_complete = false;
		_debug = false;
		_gameover = false;
		Scene::dispose();
	}
}


#pragma mark -
#pragma mark Scene Graph
/**
* Creates the scene graph.
*/
void GameScene::createSceneGraph(Size dimen) {
	_rootnode = Node::alloc();
	_rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_rootnode->setPosition(Vec2::ZERO);

	_winnode = Label::alloc("SUCCESS", _assets->get<Font>(PRIMARY_FONT));
	_winnode->setPosition(dimen / 2.0f);
	_winnode->setForeground(FONT_COLOR);
	_winnode->setVisible(false);

	_losenode = Label::alloc("FAILURE", _assets->get<Font>(PRIMARY_FONT));
	_losenode->setPosition(dimen / 2.0f);
	_losenode->setForeground(FONT_COLOR);
	_losenode->setVisible(false);

	_loadnode = Label::alloc("RESET", _assets->get<Font>(PRIMARY_FONT));
	_loadnode->setPosition(dimen / 2.0f);
	_loadnode->setForeground(FONT_COLOR);
	_loadnode->setVisible(false);

	addChild(_rootnode, 0);
	addChild(_winnode, 1);
	addChild(_losenode, 2);
	addChild(_loadnode, 3);

	_rootnode->setContentSize(dimen);

	// This root node becomes the physics world root node, all physics objects are added to this node
	_gamestate->setRootNode(_rootnode, _assets);
}


#pragma mark -
#pragma mark Physics Handling

/**
* Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
*
* @param world the physics world to activate world collision callbacks on
*/
void GameScene::activateWorldCollisions() {
	_gamestate->getWorld()->activateCollisionCallbacks(true);
	_gamestate->getWorld()->onBeginContact = [this](b2Contact* contact) {
		_collisions.beginContact(contact);
	};
	_gamestate->getWorld()->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
		_collisions.beforeSolve(contact, oldManifold);
	};
}

#pragma mark -
#pragma mark Gameplay Handling
/**
* Executes the core gameplay loop of this world.
*
* This method contains the specific update code for this mini-game. It does
* not handle collisions, as those are managed by the parent class WorldController.
* This method is called after input is read, but before collisions are resolved.
* The very last thing that it should do is apply forces to the appropriate objects.
*
* @param  dt    Number of seconds since last animation frame
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
			_gamestate->setRootNode(_rootnode, _assets); // Obtains ownership of root.
			_gamestate->showDebug(_debug);

			activateWorldCollisions();

			_loadnode->setVisible(false);
		}
		else {
			// Level is not loaded yet; refuse input
			return;
		}
	}
	_input.update(dt);

	if (_gameover || _input.didReset()) {
		reset();
	}

	if (_input.didDebug()) {
		setDebug(!isDebug());
	}

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}
    
    PlayerModel* player = _gamestate->getPlayer().get();
    if(_input.didSling(true) && player->canSling()){
        cugl::Vec2 sling = _input.getLatestSlingVector();
        player->applyLinearImpulse(sling);
    }

	// Update the physics world
	_gamestate->getWorld()->update(dt);
	_collisions.addDestroyFrictionJoints(_gamestate->getWorld());
}

/**
* Resets the status of the game so that we can play again.
*
*/
void GameScene::reset() {
	// Unload the level but keep in memory temporarily
	_assets->unload<LevelController>(PROTO_LEVEL_KEY);

	// Load a new level and quit update
	_loadnode->setVisible(true);
	_assets->load<LevelController>(PROTO_LEVEL_KEY, PROTO_LEVEL_FILE);
	setComplete(false);
	return;
}
