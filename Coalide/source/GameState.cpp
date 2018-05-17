//
//    GameState.cpp
//    Coalide
//
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"
#include "ObjectModel.h"

#include <string>

using namespace cugl;


/**
 * Clears the root scene graph node for this level
 */
void GameState::clearRootNode() {
    if (_rootnode == nullptr) {
        return;
    }
    _worldnode->removeFromParent();
    _worldnode->removeAllChildren();
    _worldnode = nullptr;
    
    _debugnode->removeFromParent();
    _debugnode->removeAllChildren();
    _debugnode = nullptr;
    
    _rootnode = nullptr;
}

/**
 * Destroys this level, releasing all resources.
 */
void GameState::dispose() {
	_world = nullptr;
	_uiNode = nullptr;
	_pauseButton = nullptr;
	_playButton = nullptr;
	_menuButton = nullptr;
	_quitButton = nullptr;
	_nextButton = nullptr;
	_restartButton = nullptr;
	_winText = nullptr;
	_gameOverScreen = nullptr;
	_gameOverText = nullptr;
	clearRootNode();
}

/**
* Sets the scene graph node for drawing purposes.
*
* @param value  the scene graph node for drawing purposes.
*
* @retain  a reference to this scene graph node
* @release the previous scene graph node used by this object
*/
void GameState::setRootNode(const std::shared_ptr<Node>& node) {
	if (_rootnode != nullptr) {
		clearRootNode();
	}

	_rootnode = node;
	_isPaused = false;

	// Create, but transfer ownership to root
	_worldnode = Node::alloc();

	_worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_worldnode->setPosition(Vec2::ZERO);

	_debugnode = Node::alloc();
	_debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
	_debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_debugnode->setPosition(Vec2::ZERO);

	_rootnode->addChild(_worldnode, 0);
	_rootnode->addChild(_debugnode, 1);

//    // Add the individual elements
//    for (int i = 0; i < _tiles.size(); i++) {
//        std::shared_ptr<TileModel> tile = _tiles[i];
//        std::shared_ptr<PolygonNode> dirtNode;
//        std::shared_ptr<PolygonNode> iceNode;
//        std::shared_ptr<PolygonNode> sandNode;
//        if(tile->getType() == TILE_TYPE::GRASS || tile->getType() == TILE_TYPE::ICE || tile->getType() == TILE_TYPE::SAND){
//            double* tileSubtexture = tile->getDirtSubTexture();
//            dirtNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getDirtTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
//        }
//        if(tile->getType() == TILE_TYPE::ICE || tile->getType() == TILE_TYPE::SAND){
//            double* tileSubtexture = tile->getIceSubTexture();
//            iceNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getIceTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
//            iceNode->setPosition(.5f *_scale.x, +.5f *_scale.y);
//            dirtNode->addChild(iceNode);
//        }
//        if(tile->getType() == TILE_TYPE::SAND){
//            double* tileSubtexture = tile->getSandSubTexture();
//            sandNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getIceTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
//            sandNode->setPosition(.5f *_scale.x, +.5f *_scale.y);
//            iceNode->addChild(sandNode);
//        }
//
//        tile->setNode(dirtNode);
//        tile->setDrawScale(_scale.x);
//        //tile->setDebugScene(_debugnode);
//
//        dirtNode->setPosition(tile->getPosition()*_scale);
//
//        _worldnode->addChild(dirtNode, TILE_PRIORITY);
//    }
	// Add the individual elements
    for (int i = 0; i < _tiles.size(); i++) {
        std::shared_ptr<TileModel> tile = _tiles[i];
        std::shared_ptr<PolygonNode> dirtNode = nullptr;
        std::shared_ptr<PolygonNode> iceNode = nullptr;
        std::shared_ptr<PolygonNode> sandNode = nullptr;
        std::shared_ptr<PolygonNode> waterDecal = nullptr;
        std::shared_ptr<PolygonNode> waterBase = nullptr;

        if(tile->hasWaterDecal()){
            double* tileSubtexture = tile->getWaterDecalSubTexture();
            waterDecal = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getWaterTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
        }
        if(tile->hasWaterBase()){
            double* tileSubtexture = tile->getWaterBaseSubTexture();
            waterBase = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getWaterTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
            if(waterDecal != nullptr){
                waterBase->setPosition(.5f *_scale.x, +.5f *_scale.y);
                waterDecal->addChild(waterBase);
            }
        }
        if(tile->getType() == TILE_TYPE::GRASS || tile->getType() == TILE_TYPE::ICE || tile->getType() == TILE_TYPE::SAND){
            double* tileSubtexture = tile->getDirtSubTexture();
            dirtNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getDirtTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
            if(waterBase!=nullptr){
                dirtNode->setPosition(.5f *_scale.x, +.5f *_scale.y);

                waterBase->addChild(dirtNode);
            } else if(waterDecal != nullptr) {
                waterDecal->addChild(dirtNode);
                dirtNode->setPosition(.5f *_scale.x, +.5f *_scale.y);
            }
        }
        if(tile->getType() == TILE_TYPE::ICE || tile->getType() == TILE_TYPE::SAND){
            double* tileSubtexture = tile->getIceSubTexture();
            iceNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getIceTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
            iceNode->setPosition(.5f *_scale.x, +.5f *_scale.y);
            dirtNode->addChild(iceNode);
        }
        if(tile->getType() == TILE_TYPE::SAND){
            double* tileSubtexture = tile->getSandSubTexture();
            sandNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getIceTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
            sandNode->setPosition(.5f *_scale.x, +.5f *_scale.y);;
            iceNode->addChild(sandNode);
        }
        if(waterDecal!=nullptr){
            tile->setNode(waterDecal);
            waterDecal->setPosition(tile->getPosition()*_scale);
            _worldnode->addChild(waterDecal, TILE_PRIORITY);
        }else if(waterBase!=nullptr){
            tile->setNode(waterBase);
            waterBase->setPosition(tile->getPosition()*_scale);
            _worldnode->addChild(waterBase, TILE_PRIORITY);
        }
    }

	if (_player != nullptr) {
        Rect nicoal_start = Rect(0.0f,448.0f,64.0f,64.0f);
        auto playerNode = PolygonNode::allocWithTexture(_assets->get<Texture>(_player->getTextureKey()), nicoal_start);
        
		//Collision sparks node
		std::shared_ptr<cugl::AnimationNode> sparks = AnimationNode::alloc(_assets->get<Texture>("sparks"), 1, 6);
		sparks->setVisible(false);
		_player->setSparks(sparks);
		playerNode->addChild(sparks);

        _player->setNode(playerNode);
		_player->setDrawScale(_scale.x);

        //Arrow indicator for Nicoal
        Rect indicator_start = Rect(0.0f,0.0f,102.0f,65.0f);
        std::shared_ptr<cugl::Node> _arrow = PolygonNode::allocWithTexture(_assets->get<Texture>("arrow_indicator"));
        _arrow->setScale(0.5f);
        std::shared_ptr<cugl::PolygonNode> _circle = PolygonNode::allocWithTexture(_assets->get<Texture>("circle_indicator"), indicator_start);
        
        _arrow->setAnchor(cugl::Vec2(2.0, 2.0));
        _arrow->setVisible(false);
        _circle->setVisible(false);
        _player->setArrow(_arrow);
        _player->setCircle(_circle);
        
        // Create the polygon node (empty, as the model will initialize)
        _worldnode->addChild(_arrow, 1);
        _worldnode->addChild(_circle, 1);
        _worldnode->addChild(playerNode, UNIT_PRIORITY);
	}

	if (_enemies.size() > 0) {
		for (auto it = _enemies.begin(); it != _enemies.end(); ++it) {
			std::shared_ptr<EnemyModel> enemy = *it;
            std::shared_ptr<PolygonNode> enemyNode = nullptr;
            if(enemy->isOnion() || enemy->isMushroom())
                enemyNode = PolygonNode::allocWithTexture(_assets->get<Texture>(enemy->getTextureKey()),Rect(0,0,128,128));
            else {
                enemyNode = PolygonNode::allocWithTexture(_assets->get<Texture>(enemy->getTextureKey()),Rect(0,0,64,64));
            }

			//Collision sparks node
			std::shared_ptr<cugl::AnimationNode> sparks = AnimationNode::alloc(_assets->get<Texture>("sparks"), 1, 6);
			sparks->setVisible(false);
			enemy->setSparks(sparks);
			enemyNode->addChild(sparks);

			enemy->setNode(enemyNode);
			enemy->setDrawScale(_scale.x);
			//enemy->setDebugScene(_debugnode);

			_worldnode->addChild(enemyNode, UNIT_PRIORITY);
		}
	}

	if (_objects.size() > 0) {
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			std::shared_ptr<ObjectModel> object = *it;
			if (object->isBreakable()) {
				std::shared_ptr<AnimationNode> objectNode = AnimationNode::alloc(_assets->get<Texture>(object->getTextureKey()), 1, 6);
				object->setNode(objectNode);
				//objectNode->setScale(.75, 1);
				_worldnode->addChild(objectNode, UNIT_PRIORITY);
			}
			else if (object->isMovable()) {
				std::shared_ptr<AnimationNode> objectNode = AnimationNode::alloc(_assets->get<Texture>(object->getTextureKey()), 1, 12);
				object->setNode(objectNode);
				//objectNode->setScale(.75, 1);
				_worldnode->addChild(objectNode, UNIT_PRIORITY);
			}
			else {
				std::shared_ptr<PolygonNode> objectNode = PolygonNode::allocWithTexture(_assets->get<Texture>(object->getTextureKey()));
				//objectNode->setScale(.75, 1);
				object->setNode(objectNode);
				_worldnode->addChild(objectNode, UNIT_PRIORITY);
			}
			
			object->setDrawScale(_scale.x);
            
			//object->setDebugScene(_debugnode);

		}
	}
    
	// create the UI buttons
	_uiNode = Node::alloc();
	_uiNode->setPosition(300, 300);
	std::shared_ptr<cugl::Node> _backNode = PolygonNode::allocWithTexture(_assets->get<Texture>("menu_button"));
	std::shared_ptr<cugl::Node> _pauseNode = PolygonNode::allocWithTexture(_assets->get<Texture>("pause_button"));
	std::shared_ptr<cugl::Node> _muteNode = PolygonNode::allocWithTexture(_assets->get<Texture>("mute_button"));
	std::shared_ptr<cugl::Node> _playNode = PolygonNode::allocWithTexture(_assets->get<Texture>("play_button"));
	std::shared_ptr<cugl::Node> _restartNode = PolygonNode::allocWithTexture(_assets->get<Texture>("restart_button"));
	std::shared_ptr<cugl::Node> _quitNode = PolygonNode::allocWithTexture(_assets->get<Texture>("quit_button"));
	std::shared_ptr<cugl::Node> _nextNode = PolygonNode::allocWithTexture(_assets->get<Texture>("restart_button"));
	_gameOverScreen = PolygonNode::allocWithTexture(_assets->get<Texture>("game_over_screen"));
	_gameOverText = PolygonNode::allocWithTexture(_assets->get<Texture>("game_over"));
	_winText = PolygonNode::allocWithTexture(_assets->get<Texture>("win"));

	float xMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxX();
	float yMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxY();

	_gameOverScreen->setVisible(false);
	_gameOverScreen->setPosition(0, 0);
	_gameOverText->setVisible(false);
	_gameOverText->setPosition(0, 75);
	_winText->setVisible(false);
	_winText->setPosition(0, 75);

	_menuButton = cugl::Button::alloc(_backNode);
	_menuButton->setPosition(0, 10000);
	_menuButton->setVisible(false);

	_pauseButton = cugl::Button::alloc(_pauseNode);
	_pauseButton->setPosition(-xMax/2.0, -yMax/2.0);
	_pauseButton->setScale(.5, .5);

	_muteButton = cugl::Button::alloc(_muteNode);
	_muteButton->setPosition(xMax / 2.0 - 50, -yMax / 2.0);
	_muteButton->setScale(.3, .3);

	_playButton = cugl::Button::alloc(_playNode);
	_playButton->setPosition(-200, 10000);
	_playButton->setVisible(false);

	_quitButton = cugl::Button::alloc(_quitNode);
	_quitButton->setPosition(-200, 10000);
	_quitButton->setVisible(false);

	_restartButton = cugl::Button::alloc(_restartNode);
	_restartButton->setPosition(-200, 10000);
	_restartButton->setVisible(false);

	_nextButton = cugl::Button::alloc(_nextNode);
	_nextButton->setPosition(-200, 10000);
	_nextButton->setVisible(false);

	_menuButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickMenu = true;
		}
	});

	_pauseButton->setListener([=](const std::string& name, bool down) {
		if (!down && !_gameOverScreen->isVisible() && !_gameOverScreen->isVisible()) { // add win screen
			_isPaused = true;
			_playButton->setVisible(true);
			_playButton->setPosition(-200, -100);
			_menuButton->setVisible(true);
			_menuButton->setPosition(0, -100);
			_pauseButton->setVisible(false);
		}
	});

	_playButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_isPaused = false;
			_playButton->setVisible(false);
			_playButton->setPosition(-200, 10000);
			_menuButton->setVisible(false);
			_menuButton->setPosition(0, 10000);
			_pauseButton->setVisible(true);
		}
	});

	_quitButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickMenu = true;
		}
	});

	_restartButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickRestart = true;
		}
	});

	_nextButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickNext = true;
		}
	});

	_muteButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickMute = true;
		}
	});

	_menuButton->activate(5);
	_pauseButton->activate(6);
	_muteButton->activate(7);
	_playButton->activate(8);
	_quitButton->activate(9);
	_restartButton->activate(10);
	_nextButton->activate(11);
	_uiNode->addChild(_menuButton, UNIT_PRIORITY);
	_uiNode->addChild(_pauseButton, UNIT_PRIORITY);
	_uiNode->addChild(_muteButton, UNIT_PRIORITY);
	_uiNode->addChild(_playButton, UNIT_PRIORITY);
	_uiNode->addChild(_gameOverScreen, UNIT_PRIORITY);
	_uiNode->addChild(_gameOverText, UNIT_PRIORITY);
	_uiNode->addChild(_winText, UNIT_PRIORITY);
	_uiNode->addChild(_quitButton, UNIT_PRIORITY);
	_uiNode->addChild(_nextButton, UNIT_PRIORITY);
	_uiNode->addChild(_restartButton, UNIT_PRIORITY);
	
	_worldnode->addChild(_uiNode, 10000);
}

void GameState::addSporeNode(std::shared_ptr<EnemyModel> spore) {
    auto sporeNode = PolygonNode::allocWithTexture(_assets->get<Texture>(spore->getTextureKey()));
    spore->setNode(sporeNode);
    spore->setDrawScale(_scale.x);
    //spore->setDebugScene(_debugnode);
    
    _worldnode->addChild(sporeNode, UNIT_PRIORITY);
}

void GameState::showGameOverScreen(bool showing) {
	if (showing) {
		_quitButton->setVisible(true);
		_quitButton->setPosition(-150, -50);
		_restartButton->setVisible(true);
		_restartButton->setPosition(30, -50);
		_gameOverScreen->setVisible(true);
		_gameOverText->setVisible(true);
	}
	else {
		_quitButton->setVisible(false);
		_quitButton->setPosition(-200, 10000);
		_restartButton->setVisible(false);
		_restartButton->setPosition(0, 10000);
		_gameOverScreen->setVisible(false);
		_gameOverText->setVisible(false);
	}
}

void GameState::showWinScreen(bool showing) {
	if (showing) {
		_quitButton->setVisible(true);
		_quitButton->setPosition(-150, -50);
		_nextButton->setVisible(true);
		_nextButton->setPosition(30, -50);
		_gameOverScreen->setVisible(true);
		_winText->setVisible(true);
	}
	else {
		_quitButton->setVisible(false);
		_quitButton->setPosition(-200, 10000);
		_nextButton->setVisible(false);
		_nextButton->setPosition(0, 10000);
		_gameOverScreen->setVisible(false);
		_winText->setVisible(false);
	}
}

/**
 * Toggles whether to show the debug layer of this game world.
 *
 * The debug layer displays wireframe outlines of the physics fixtures.
 *
 * @param  flag whether to show the debug layer of this game world
 */
void GameState::showDebug(bool flag) {
    if (_debugnode != nullptr) {
        _debugnode->setVisible(flag);
    }
}
