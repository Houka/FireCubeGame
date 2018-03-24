//
//  GameScene.cpp
//  Coalide
//
#include "GameScene.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "Constants.h"

#include <string>

using namespace cugl;

#define SLOW_MOTION .00001
#define NORMAL_MOTION .015
#define MAX_PLAYER_SPEED 20

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

	_enemyCount = _gamestate->getEnemies().size();

	// Activate the collision callbacks for the physics world
	activateWorldCollisions();

	// Set up the scene graph
	createSceneGraph(dimen);

	// initialize the camera
	cugl::Vec2 gameCenter = _gamestate->getBounds().size * 32. / 2.;
	cugl::Vec2 cameraPos = getCamera()->getPosition();
	getCamera()->translate(gameCenter - cameraPos);
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

	_loadnode = Label::alloc("", _assets->get<Font>(PRIMARY_FONT));
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
	if (_reloading) {
		if (_assets->complete()) {
			_gamestate = nullptr;

			// Access and initialize level
			_gamestate = _assets->get<LevelController>(PROTO_LEVEL_KEY)->getGameState();
			activateWorldCollisions();

			Size dimen = Application::get()->getDisplaySize();
			dimen *= GAME_WIDTH / dimen.width;

			createSceneGraph(dimen);

			_enemyCount = _gamestate->getEnemies().size();

			_reloading = false;

			//_loadnode->setVisible(false);
		}
		else {
			// Level is not loaded yet; refuse input
			return;
		}
	}
	_input.update(dt);

	if (_gameover || _input.didReset()) {
		reset();
		return;
	}

	if (_complete) {
		reset();
		//_winnode->setVisible(true);
		return;
	}

	if (_input.didDebug()) {
		setDebug(!isDebug());
	}

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

    ObstacleWorld* world = _gamestate->getWorld().get();
    PlayerModel* player = _gamestate->getPlayer().get();

    // Touch input for sling is in pogress and sets the time slowing mechanic
    if(_input.didStartSling() && player->canSling() &&
       std::abs(world->getStepsize() - NORMAL_MOTION) < SLOW_MOTION){
        world->setStepsize(SLOW_MOTION);
        player->setColor(Color4::ORANGE);
		// update the aim arrow
		player->updateArrow(_input.getCurrentAim(), true);
    } else if(std::abs(world->getStepsize() - SLOW_MOTION) < SLOW_MOTION){
        world->setStepsize(NORMAL_MOTION);
        player->setColor(Color4::WHITE);
    }

    // Applies vector from touch input to player and set to charging state
    if(_input.didSling(true) && player->canSling()){
        cugl::Vec2 sling = _input.getLatestSlingVector();
        player->applyLinearImpulse(sling);
        player->setCharging(true);
		player->updateArrow(false);
    }
    
    // Caps player speed to MAX_PLAYER SPEED
    if(player->getLinearVelocity().length() >= MAX_PLAYER_SPEED){
        Vec2 capped_speed = player->getLinearVelocity().normalize().scale(MAX_PLAYER_SPEED);
        player->setLinearVelocity(capped_speed);
    }
    
    // Changes player state from charging if below speed threshold
    if(player->getLinearVelocity().length() < MIN_SPEED_FOR_CHARGING){
        player->setCharging(false);
    }
    
    // Applies movement vector to all enemies curently alive in the game and sets them to charging state
    if(_enemyCount != 0) {
        std::vector<std::tuple<EnemyModel*, Vec2>> enemiesToMove = _ai.getEnemyMoves(_gamestate);
        for(std::tuple<EnemyModel*, Vec2> pair : enemiesToMove){
            EnemyModel* enemy = std::get<0>(pair);
            Vec2 sling = std::get<1>(pair);
            enemy->applyLinearImpulse(sling);
        }
    }

	Vec2 player_pos = player->getPosition();
    Size gameBounds = _gamestate->getBounds().size;
    
    CULog("\nGame Width: %d, Game Height: %d \nPlayer Position: %s \nPlayer in Bounds: %d", gameBounds.getIWidth(), gameBounds.getIHeight(), player_pos.toString().c_str(), player->inBounds(gameBounds.getIWidth(), gameBounds.getIWidth()));
    
    // LEVEL DEATH: Sets friction for player and checks if in bounds/death conditions for the game
    if (player->inBounds(gameBounds.getIWidth(), gameBounds.getIHeight())) {
		float friction = _gamestate->getBoard()[(int)floor(player_pos.y)][(int)floor(player_pos.x)];
        if(!player->getCharging()){
            player->setFriction(friction);
        } else {
            player->setFriction(0.0001f);
        }
		if (friction == 0 && !player->getCharging()) {
			_gameover = true;
		}
	}
	else {
		player->setFriction(0);
        player->setCharging(false);
        _gameover = true;
	}

    // Loops through enemies and sets friction and also checks for in bounds/death conditions
	for (int i = 0; i < _gamestate->getEnemies().size(); i++) {
		EnemyModel* enemy = _gamestate->getEnemies()[i].get();
		Vec2 enemy_pos = enemy->getPosition();
		if (enemy_pos.x > 0 && enemy_pos.y > 0 && enemy_pos.x < _gamestate->getBounds().size.getIWidth() && enemy_pos.y < _gamestate->getBounds().size.getIHeight()) {
			float friction = _gamestate->getBoard()[(int)floor(enemy_pos.y)][(int)floor(enemy_pos.x)];
			enemy->setFriction(friction);

			if (friction == 0) {
				removeEnemy(enemy);
			}
		}
		else {
			enemy->setFriction(0);
		}
	}

    // LEVEL COMPLETE: If all enemies are dead then level completed
	if (_enemyCount == 0) {
		//_complete = true;
	}

	// Update the physics world
	_gamestate->getWorld()->update(dt);

	// update the camera
	player->getNode()->getScene()->setOffset(cugl::Vec2(0,0));
	cugl::Vec2 cameraPos = player->getNode()->getScene()->getCamera()->getPosition();
	cugl::Vec2 playerPos = player->getNode()->getPosition();
	float cameraTransX;
	float cameraTransY;
	
	cugl::Vec2 gameBound = _gamestate->getBounds().size*32;
	float xMax = player->getNode()->getScene()->getCamera()->getViewport().getMaxX();
	float yMax = player->getNode()->getScene()->getCamera()->getViewport().getMaxY();
	
	cugl::Vec2 boundBottom = Scene::screenToWorldCoords(cugl::Vec2());
	cugl::Vec2 boundTop = Scene::screenToWorldCoords(cugl::Vec2(xMax,yMax));

	cameraTransX = playerPos.x - cameraPos.x;
	cameraTransY = playerPos.y - cameraPos.y;

	// smooth pan
	if (std::abs(cameraTransX) > 5) {
		cameraTransX *= .01;
	}

	if (std::abs(cameraTransY) > 5) {
		cameraTransY *= .01;
	}

	if ((boundBottom.x < 0 && cameraTransX < 0) || (boundTop.x > gameBound.x && cameraTransX > 0 )) {
		cameraTransX = 0;
	}
	
	if ((boundTop.y < 0 && cameraTransY < 0) || (boundBottom.y > gameBound.y && cameraTransY > 0)) {
		cameraTransY = 0;
	}


	player->getNode()->getScene()->getCamera()->translate(cugl::Vec2(cameraTransX,cameraTransY));
}

void GameScene::removeEnemy(EnemyModel* enemy) {
	// do not attempt to remove a bullet that has already been removed
	if (enemy->isRemoved()) {
		return;
	}
	_gamestate->getRootNode()->getChild(0)->removeChild(enemy->getNode());
	enemy->setDebugScene(nullptr);
	enemy->markRemoved(true);
	_enemyCount--;
}

/**
* Resets the status of the game so that we can play again.
*
*/
void GameScene::reset() {
	// Unload the level but keep in memory temporarily
	_assets->unload<LevelController>(PROTO_LEVEL_KEY);

	// Load a new level and quit update
	//_loadnode->setVisible(true);
	_reloading = true;
	_assets->load<LevelController>(PROTO_LEVEL_KEY, PROTO_LEVEL_FILE);
	setComplete(false);
	_gameover = false;
	
	return;
}
