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

/** Define the time settings for animation */
#define DURATION 1.0f
#define DISTANCE 200
#define REPEATS  1
#define ACT_KEY  "current"

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, InputController input, std::string levelKey) {
    //set application to right color
    Application::get()->setClearColor(Color4(86,210,216,255));
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

	_levelKey = levelKey;

	// Get the loaded level
	_gamestate = assets->get<LevelController>(levelKey)->getGameState();
	
	if (_gamestate == nullptr) {
		CULog("Fail!");
		return false;
	}

	// Initialize the controllers used in the game mode
	_collisions.init(_assets);
	_ai.init(_gamestate);
	_input.init();

	_enemyCount = _gamestate->getEnemies().size();

	// Activate the collision callbacks for the physics world
	activateWorldCollisions();

	// Set up the scene graph
	createSceneGraph(dimen);

	_gamestate->showGameOverScreen(false);
	_gamestate->showWinScreen(false);
	_gamestate->resetDidClickMenu();
	_gamestate->resetDidClickRestart();
	_gamestate->resetDidClickNext();
	_gamestate->resetDidClickMute();
	_gameover = false;
	_complete = false;

	// initialize the camera
	cugl::Vec2 gameCenter = _gamestate->getBounds().size * 64. / 2.;
	cugl::Vec2 cameraPos = getCamera()->getPosition();
	getCamera()->translate(gameCenter - cameraPos);
    
    counter = 0;
    deltaImage = 0.0f;
    up = true;
    
    std::srand(time(NULL));
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
	_gamestate->setAssets(_assets);
	_gamestate->setRootNode(_rootnode);
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

void GameScene::pause() {
	_gamestate->didPause();
}
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

	if (_gamestate->isPaused()) {
		return;
	}

	// Check to see if new level loaded yet
	if (_reloading) {
		if (_assets->complete()) {
			_gamestate = nullptr;

			// Access and initialize level
			_gamestate = _assets->get<LevelController>(_levelKey)->getGameState();
			activateWorldCollisions();

			Size dimen = Application::get()->getDisplaySize();
			dimen *= GAME_WIDTH / dimen.width;

			createSceneGraph(dimen);

			_enemyCount = _gamestate->getEnemies().size();

			_reloading = false;
		}
		else {
			// Level is not loaded yet; refuse input
			return;
		}
	}
	_input.update(dt);

	_gamestate->resetDidClickMute();
	_gamestate->resetDidClickMenu();
	_gamestate->resetDidClickNext();
	_gamestate->resetDidClickRestart();

	if (_gameover) {
		_gamestate->showGameOverScreen(true);
		return;
	}

	if (_complete) {
		_gamestate->showWinScreen(true);
		return;
	}

	if (_input.didDebug()) {
		setDebug(!isDebug());
	}

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

    std::shared_ptr<ObstacleWorld> world = _gamestate->getWorld();
    std::shared_ptr<PlayerModel> player = _gamestate->getPlayer();
    Size gameBounds = _gamestate->getBounds().size;
    Vec2 player_pos = player->getPosition();
    Vec2 currentAim = _input.getCurrentAim();
    // Much easier to work in degrees...
    float angle = currentAim.getAngle() * 180.0f / 3.14159f;

    // Touch input for sling is in pogress and sets the time slowing mechanic
    if(_input.didStartSling() && !player->isStunned()){
        world->setStepsize(SLOW_MOTION);
        cugl::Vec2 pan = _input.getCameraPan();
        
        if(!player->getCharging() && pan.length() == 0){
            // changes texture of nicoal
            player->setDirectionTexture(angle, 0);
            // update the aim arrow
            Vec2 clampedAim = _input.getCurrentAim();
            if (clampedAim.length() > _input.getMaxSling()) {
                clampedAim.scale(1.0 / clampedAim.length() * _input.getMaxSling());
            }
            player->updateArrow(clampedAim, player->getNode(), true);
            player->updateCircle(clampedAim, player->getNode(), true);
            if(_input.getCurrentAim().length() > 200.0f) {
                player->setDirectionTexture(angle, 1);
            }
        }
    } else if(std::abs(world->getStepsize() - SLOW_MOTION) < SLOW_MOTION){
        world->setStepsize(NORMAL_MOTION);
        player->setColor(Color4::WHITE);
    }

    // Applies vector from touch input to player and set to charging state
    if(_input.didSling(true) && player->canSling() && !player->isStunned()){
        float angle = currentAim.getAngle() * 180.0f / 3.14159f;
        cugl::Vec2 sling = _input.getLatestSlingVector();
        player->applyLinearImpulse(sling);
        player->setCharging(true);
        // changes texture of nicoal
        player->setDirectionTexture(angle, 2);
    }

    // Caps player speed to MAX_PLAYER SPEED
    if(player->getLinearVelocity().length() >= MAX_PLAYER_SPEED){
        Vec2 capped_speed = player->getLinearVelocity().normalize().scale(MAX_PLAYER_SPEED);
        player->setLinearVelocity(capped_speed);
    }
    
    if(player->getCoalided() && player->getLinearVelocity().length() <= 0.0f){
        if(player->isStunned()){
            player->setDirectionTexture(player->getPlayerDirection(), 7);
        } else {
            player->setDirectionTexture(player->getPlayerDirection(), 0);
            player ->setCoalided(false);
        }
    }

    // Changes player state from charging if below speed threshold
    if(player->getCharging() && player->getLinearVelocity().length() < MIN_SPEED_FOR_CHARGING){
        player->setCharging(false);
        player->_isSliding = true;
        player->updateCircle(false);
        // changes texture of nicoal
        player->setDirectionTexture(angle, 3);
    }
    
    if(player->_isSliding && player->getLinearVelocity().isNearZero()){
        player->_isSliding = false;
        player->updateCircle(false);
        // changes texture of nicoal
        player->setDirectionTexture(angle, 0);
    }
    
	if (player->didFall()) {
		player->setDirectionTexture(player->getPlayerDirection(), 8);
		player->_drownTimer -= 1;
		if (player->_drownTimer <= 0) {
			_gameover = true;
		}
	}

    if(!player->canSling() || _input.getCameraPan().length()) {
        player->updateArrow(false);
        player->updateCircle(false);
    }
    
    // Applies movement vector to all enemies curently alive in the game and sets them to charging state
	if (_enemyCount != 0) {
		std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> enemiesToMove = _ai.getEnemyMoves(_gamestate);
		for (std::tuple<std::shared_ptr<EnemyModel>, Vec2> pair : enemiesToMove) {
			std::shared_ptr<EnemyModel> enemy = std::get<0>(pair);
			Vec2 sling = std::get<1>(pair);
			enemy->applyLinearImpulse(sling);
			float angle = sling.getAngle(Vec2(-1.0f, 0.0f)) * 180.0f / 3.14159;

			if (!enemy->isSpore()) {
				enemy->setCharging(true);
				enemy->setDirectionTexture(angle, enemy->isAcorn(), 2);
			}
		}
	}

	for (int i = 0; i < _gamestate->getEnemies().size(); i++) {
		std::shared_ptr<EnemyModel> enemy = _gamestate->getEnemies()[i];
		if (enemy->getCharging() && enemy->getLinearVelocity().length() < MIN_SPEED_FOR_CHARGING) {
			enemy->setCharging(false);
			enemy->setSliding(true);
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 3);
		}

		if (enemy->isSliding() && enemy->getLinearVelocity().isNearZero()) {
			enemy->setSliding(false);
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 0);
		}

		if (enemy->getCoalided() && enemy->getLinearVelocity().length() <= 0.0f) {
			if (enemy->isStunned()) {
				enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 6);
			}
			else {
				enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 0);
				enemy->setCoalided(false);
			}
		}

		if (enemy->isPrepping()) {
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 1);
		}

		if (!enemy->isRemoved() && enemy->didFall()) {
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 7);
			enemy->_drownTimer -= 1;
			if (enemy->_drownTimer <= 0) {
				removeEnemy(enemy);
				_enemyCount--;
			}
		}
	}
    
	if (player->getSparky()) {
		player->updateSparks(true);
		player->setSparky(false);
		if (player->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
			player->setCharging(true);
			player->setDirectionTexture(player->getPlayerDirection(), 5);
		}
	}
	else {
		player->updateSparks();
	}

	for (int i = 0; i < _gamestate->getEnemies().size(); i++) {
		std::shared_ptr<EnemyModel> enemy = _gamestate->getEnemies()[i];
		if (!enemy->isRemoved() && enemy->getSparky()) {
			enemy->updateSparks(true);
			enemy->setSparky(false);
			if (enemy->getLinearVelocity().length() > MIN_SPEED_FOR_CHARGING) {
				enemy->setCharging(true);
				enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 5);
			}
		}
		else {
			enemy->updateSparks();
		}
	}

	updateFriction();

	bool noSmoothPan = false;
	// Super collisions
	/*if (player->isSuperCollide()) {
		world->setStepsize(SUPER_COLLISION_MOTION);
		if (getCamera()->getZoom() < 3) {
			getCamera()->setZoom(getCamera()->getZoom() + 0.08);
			noSmoothPan = true;
		}
	}
	else if (getCamera()->getZoom() > 1) {
		world->setStepsize(NORMAL_MOTION);
		getCamera()->setZoom(getCamera()->getZoom() - 0.16);
		noSmoothPan = true;
	}*/

    // LEVEL COMPLETE: If all enemies are dead then level completed
    if (_enemyCount == 0) {
        _complete = true;
		getCamera()->setZoom(1);
    }

	// Resort draw order
	player->getNode()->setZOrder((_gamestate->getBounds().size.height - player->getPosition().y)*100);

	for (int i = 0; i < _gamestate->getEnemies().size(); i++) {
		std::shared_ptr<EnemyModel> enemy = _gamestate->getEnemies()[i];
		enemy->getNode()->setZOrder((_gamestate->getBounds().size.height - enemy->getPosition().y)*100);
	}

	for (int i = 0; i < _gamestate->getObjects().size(); i++) {
		std::shared_ptr<ObjectModel> object = _gamestate->getObjects()[i];
		object->getNode()->setZOrder((_gamestate->getBounds().size.height - object->getPosition().y)*100);
	}

	for (int i = 0; i < _gamestate->getSpores().size(); i++) {
		std::shared_ptr<EnemyModel> spore = _gamestate->getSpores()[i];
		spore->getNode()->setZOrder((_gamestate->getBounds().size.height - spore->getPosition().y) * 100);
	}

	_gamestate->getWorldNode()->sortZOrder();

    // Update the physics world
    _gamestate->getWorld()->update(dt);
    
	for (int i = 0; i < _gamestate->getSpores().size(); i++) {
		std::shared_ptr<EnemyModel> spore = _gamestate->getSpores()[i];
		Vec2 spore_pos = spore->getPosition();

		if (spore->isDestroyed()) {
			removeEnemy(spore);
		}

		/** Need to remove spore from spore list? */
		else if (!(spore_pos.x > 0 && spore_pos.y > 0 && spore_pos.x < _gamestate->getBounds().size.getIWidth() && spore_pos.y < _gamestate->getBounds().size.getIHeight())) {
			removeEnemy(spore);
		}

		else if (spore->isDispersing()) {
			spore->animateSpore();
		}
	}

    for (int i = 0; i < _gamestate->getObjects().size(); i++) {
        std::shared_ptr<ObjectModel> object = _gamestate->getObjects()[i];
        if (object->isBroken()) {
            removeObject(object);
        }
		else if (object->isAnimating()) {
			object->animate();
		}
    }
    
    _gamestate->getWorld()->garbageCollect();

	// update the camera
	player->getNode()->getScene()->setOffset(cugl::Vec2(0,0));
	cugl::Vec2 cameraPos = getCamera()->getPosition();
	cugl::Vec2 playerPos = player->getNode()->getPosition();
	float cameraTransX = 0;
	float cameraTransY = 0;
	
	//cugl::Vec2 gameBound = cugl::Vec2(_gamestate->getBounds().size.getIWidth(), _gamestate->getBounds().size.getIHeight());
	cugl::Vec2 gameBound = _gamestate->getBounds().size * 64;
	float xMax = getCamera()->getViewport().getMaxX();
	float yMax = getCamera()->getViewport().getMaxY();
	
	cugl::Vec2 boundBottom = Scene::screenToWorldCoords(cugl::Vec2());
	cugl::Vec2 boundTop = Scene::screenToWorldCoords(cugl::Vec2(xMax,yMax));

	cugl::Vec2 pan = _input.getCameraPan();

	if (pan.length() > 0) {
		cameraTransX = pan.x;
		cameraTransY = pan.y;
	}
	else {
		cameraTransX = playerPos.x - cameraPos.x;
		cameraTransY = playerPos.y - cameraPos.y;

		// smooth pan
		if (!noSmoothPan) {
			if (std::abs(cameraTransX) > 5) {
				cameraTransX *= .05;
			}

			if (std::abs(cameraTransY) > 5) {
				cameraTransY *= .05;
			}
		}
	}

	if (!noSmoothPan) {
		if ((boundBottom.x < 0 && cameraTransX < 0) || (boundTop.x > gameBound.x && cameraTransX > 0)) {
			cameraTransX = 0;
		}

		if ((boundTop.y < 0 && cameraTransY < 0) || (boundBottom.y > gameBound.y && cameraTransY > 0)) {
			cameraTransY = 0;
		}
	}
    
    int cameraShakeAmplitude = 40 * (player->getCameraShakeAmplitude());
    if(cameraShakeAmplitude != 0){
        cameraTransX += rand() % cameraShakeAmplitude - (cameraShakeAmplitude/2);
        cameraTransY += rand() % cameraShakeAmplitude - (cameraShakeAmplitude/2);
    }
	_gamestate->setUIPosition(getCamera()->getPosition());
	getCamera()->translate(cugl::Vec2(round(cameraTransX),round(cameraTransY)));
	
}

void GameScene::updateFriction() {
	PlayerModel* player = _gamestate->getPlayer().get();
	Vec2 player_pos = player->getPosition();
	Size gameBounds = _gamestate->getBounds().size;

    // LEVEL DEATH: Sets friction for player and checks if in bounds/death conditions for the game
    if (player->inBounds(gameBounds.getIWidth(), gameBounds.getIHeight())) {
        if (!player->getCharging()) {
            float friction = _gamestate->getBoard()[std::max(0, (int)floor(player_pos.y-.25))][(int)floor(player_pos.x)];
			if (friction == 0 && !player->didFall()) {
				/*if (_gamestate->getBoard()[std::max(0, (int)floor(player_pos.y-.25))][std::min(gameBounds.getIWidth()-1, (int)floor(player_pos.x)-1)]) {
					player->setPosition(player_pos.x + .3, player_pos.y);
				}
				if (_gamestate->getBoard()[std::max(0, (int)floor(player_pos.y-.25))][std::min(gameBounds.getIWidth()-1, (int)floor(player_pos.x+.5))]) {
					player->setPosition(player_pos.x - .3, player_pos.y);
				}
				if (_gamestate->getBoard()[std::min(gameBounds.getIHeight()-1, (int)floor(player_pos.y+.25))][(int)floor(player_pos.x)]) {
					player->setPosition(player_pos.x, player_pos.y - .3);
				}*/

				player->setFell();
				player->setDirectionTexture(player->getPlayerDirection(), 8);
			}
            else if (friction != player->getFriction()) {
                player->setFriction(friction);
            }
        }
        else if (player->getFriction() > .1f) {
            player->setFriction(0);
        }
    }
    else {
        player->setFriction(0);
        player->setCharging(false);
        player->setDirectionTexture(player->getPlayerDirection(), 8);
        _gameover = true;
    }

	// Loops through enemies and sets friction and also checks for in bounds/death conditions
	for (int i = 0; i < _gamestate->getEnemies().size(); i++) {
		std::shared_ptr<EnemyModel> enemy = _gamestate->getEnemies()[i];
        if(enemy->isRemoved())
            continue;
		Vec2 enemy_pos = enemy->getPosition();
		if (!enemy->isSpore() && enemy_pos.x > 0 && enemy_pos.y > 0 && enemy_pos.x < _gamestate->getBounds().size.getIWidth() && enemy_pos.y < _gamestate->getBounds().size.getIHeight()) {
			float friction = _gamestate->getBoard()[(int)floor(enemy_pos.y)][(int)floor(enemy_pos.x)];
            if(!enemy->getCharging()) {
                if(friction == 0 && !enemy->didFall()) {
					enemy->setFell();
					enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 7);
                }
                else if(friction != enemy->getFriction()) {
                    enemy->setFriction(friction);
                }
            }
			else if (enemy->getFriction() > .1f) {
				enemy->setFriction(0);
			}
        }
        else if (!enemy->didFall()) {
			enemy->setFell();
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 7);
		}
		else {
			enemy->setFriction(0);
			enemy->setCharging(false);
			enemy->setDirectionTexture(enemy->getDirection(), enemy->isAcorn(), 7);
		}
        
        // Caps enemy speed to MAX_PLAYER SPEED
        /*if(enemy->getLinearVelocity().length() >= MAX_PLAYER_SPEED){
            Vec2 capped_speed = enemy->getLinearVelocity().normalize().scale(MAX_PLAYER_SPEED);
            enemy->setLinearVelocity(capped_speed);
        }*/
        
        // Changes enemy state from charging if below speed threshold
        if(enemy->getCharging() && enemy->getLinearVelocity().length() < MIN_SPEED_FOR_CHARGING){
            enemy->setCharging(false);
		}
		else {
			enemy->setCharging(true);
		}
	}

	// Loops through objects and sets friction and also checks for in bounds/death conditions
	for (int i = 0; i < _gamestate->getObjects().size(); i++) {
		std::shared_ptr<ObjectModel> object = _gamestate->getObjects()[i];
		Vec2 object_pos = object->getPosition();
		if (object_pos.x > 0 && object_pos.y > 0 && object_pos.x < _gamestate->getBounds().size.getIWidth() && object_pos.y < _gamestate->getBounds().size.getIHeight()) {
			float friction = _gamestate->getBoard()[(int)floor(object_pos.y)][(int)floor(object_pos.x)];
			if (friction == 0) {
				if (object->isMovable()) {
					object->animate();
				}
			}
			else if (friction != object->getFriction()) {
				object->setFriction(friction);
			}
		}
		else {
			object->setFriction(0);
		}
	}
}

void GameScene::removeEnemy(std::shared_ptr<EnemyModel> enemy) {
	// do not attempt to remove a bullet that has already been removed
	if (enemy->isRemoved()) {
		return;
	}
	_gamestate->getRootNode()->getChild(0)->removeChild(enemy->getNode());
	enemy->setDebugScene(nullptr);
	enemy->markRemoved(true);
}

void GameScene::removeObject(std::shared_ptr<ObjectModel> object) {
	// do not attempt to remove a bullet that has already been removed
	if (object->isRemoved()) {
		return;
	}
	_gamestate->getRootNode()->getChild(0)->removeChild(object->getNode());
	object->setDebugScene(nullptr);
	object->markRemoved(true);
}

/**
* Resets the status of the game so that we can play again.
*
*/
void GameScene::reset(const std::string& file) {
	// Unload the level but keep in memory temporarily
	_assets->unload<LevelController>(_levelKey);

	// Load a new level and quit update
	//_loadnode->setVisible(true);
	_reloading = true;
	_assets->load<LevelController>(_levelKey, file);
	_gamestate = _assets->get<LevelController>(_levelKey)->getGameState();
	setComplete(false);
	_gameover = false;
	_complete = false;
	_gamestate->resetDidClickMenu();

	_ai.init(_gamestate);
    
    // reset the camera
    //cugl::Vec2 gameCenter = _gamestate->getBounds().size * 64. / 2.;
    //cugl::Vec2 cameraPos = getCamera()->getPosition();
    //getCamera()->translate(gameCenter - cameraPos);
	
	return;
}
