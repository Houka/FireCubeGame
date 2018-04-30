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

	CULog("switching to game scene");

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
        auto playerNode = PolygonNode::allocWithTexture(_assets->get<Texture>(_player->getTextureKey()));
        auto standingPlayerNode_f = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_f"));
        auto standingPlayerNode_fls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_fls"));
        auto standingPlayerNode_l = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_l"));
        auto standingPlayerNode_bls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_bls"));
        auto standingPlayerNode_b = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_b"));
        auto standingPlayerNode_brs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_brs"));
        auto standingPlayerNode_r = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_r"));
        auto standingPlayerNode_frs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_nicoal_frs"));
        
        auto chargingPlayerNode_f = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_f"));
        auto chargingPlayerNode_fls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_fls"));
        auto chargingPlayerNode_l = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_l"));
        auto chargingPlayerNode_bls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_bls"));
        auto chargingPlayerNode_b = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_b"));
        auto chargingPlayerNode_brs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_brs"));
        auto chargingPlayerNode_r = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_r"));
        auto chargingPlayerNode_frs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging_frs"));
        
        auto slidingPlayerNode_f = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_f"));
        auto slidingPlayerNode_fls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_fls"));
        auto slidingPlayerNode_l = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_l"));
        auto slidingPlayerNode_bls = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_bls"));
        auto slidingPlayerNode_b = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_b"));
        auto slidingPlayerNode_brs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_brs"));
        auto slidingPlayerNode_r = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_r"));
        auto slidingPlayerNode_frs = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_sliding_frs"));
        
        _player->setNode(playerNode);
        _player->setTextNode(standingPlayerNode_f, 0, 0, true);
        _player->setTextNode(standingPlayerNode_fls, 0, 1, true);
        _player->setTextNode(standingPlayerNode_l, 0, 2, true);
        _player->setTextNode(standingPlayerNode_bls, 0, 3, true);
        _player->setTextNode(standingPlayerNode_b, 0, 4, true);
        _player->setTextNode(standingPlayerNode_brs, 0, 5, true);
        _player->setTextNode(standingPlayerNode_r, 0, 6,true);
        _player->setTextNode(standingPlayerNode_frs, 0, 7, true);
        
        _player->setTextNode(chargingPlayerNode_f, 2, 0, true);
        _player->setTextNode(chargingPlayerNode_fls, 2, 1, true);
        _player->setTextNode(chargingPlayerNode_l, 2, 2, true);
        _player->setTextNode(chargingPlayerNode_bls, 2, 3, true);
        _player->setTextNode(chargingPlayerNode_b, 2, 4, true);
        _player->setTextNode(chargingPlayerNode_brs, 2, 5, true);
        _player->setTextNode(chargingPlayerNode_r, 2, 6,true);
        _player->setTextNode(chargingPlayerNode_frs, 2, 7, true);
        
        _player->setTextNode(slidingPlayerNode_f, 3, 0, true);
        _player->setTextNode(slidingPlayerNode_fls, 3, 1, true);
        _player->setTextNode(slidingPlayerNode_l, 3, 2, true);
        _player->setTextNode(slidingPlayerNode_bls, 3, 3, true);
        _player->setTextNode(slidingPlayerNode_b, 3, 4, true);
        _player->setTextNode(slidingPlayerNode_brs, 3, 5, true);
        _player->setTextNode(slidingPlayerNode_r, 3, 6,true);
        _player->setTextNode(slidingPlayerNode_frs, 3, 7, true);
		_player->setDrawScale(_scale.x);
		//_player->setDebugScene(_debugnode);

        //Arrow indicator for Nicoal
        std::shared_ptr<cugl::Node> _arrow = PolygonNode::allocWithTexture(_assets->get<Texture>("arrow_indicator"));

//        const std::vector<cugl::Vec2> arrowLine = { cugl::Vec2(0,0), cugl::Vec2(0, 2) };
//        std::shared_ptr<cugl::PathNode> _arrow = PathNode::allocWithVertices(arrowLine, 1, cugl::PathJoint::NONE, cugl::PathCap::NONE, false);
//        _arrow->setAnchor(cugl::Vec2(0.0, 0.0));
        _arrow->setVisible(false);
        _player->setArrow(_arrow);
        
        // Create the polygon node (empty, as the model will initialize)
        playerNode->addChild(_arrow, UNIT_PRIORITY);
        _worldnode->addChild(playerNode, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_f, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_fls, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_l, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_bls, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_b, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_brs, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_r, UNIT_PRIORITY);
        _worldnode->addChild(standingPlayerNode_frs, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_f, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_fls, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_l, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_bls, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_b, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_brs, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_r, UNIT_PRIORITY);
        _worldnode->addChild(chargingPlayerNode_frs, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_f, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_fls, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_l, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_bls, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_b, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_brs, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_r, UNIT_PRIORITY);
        _worldnode->addChild(slidingPlayerNode_frs, UNIT_PRIORITY);

	}

	if (_enemies.size() > 0) {
		for (auto it = _enemies.begin(); it != _enemies.end(); ++it) {
			std::shared_ptr<EnemyModel> enemy = *it;
			auto enemyNode = PolygonNode::allocWithTexture(_assets->get<Texture>(enemy->getTextureKey()));
			enemy->setNode(enemyNode);
			enemy->setDrawScale(_scale.x);
			//enemy->setDebugScene(_debugnode);

			_worldnode->addChild(enemyNode, UNIT_PRIORITY);
		}
	}

	if (_objects.size() > 0) {
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			std::shared_ptr<ObjectModel> object = *it;
			auto objectNode = PolygonNode::allocWithTexture(_assets->get<Texture>(object->getTextureKey()), Rect(0, 0, 64, 64));
			object->setNode(objectNode);
			object->setDrawScale(_scale.x);
            objectNode->setScale(.75, 1);
			//object->setDebugScene(_debugnode);

			_worldnode->addChild(objectNode, UNIT_PRIORITY);
		}
	}
    
	// create the UI buttons
	_uiNode = Node::alloc();
	_uiNode->setPosition(300, 300);
	std::shared_ptr<cugl::Node> _backNode = PolygonNode::allocWithTexture(_assets->get<Texture>("menu_button"));
	std::shared_ptr<cugl::Node> _pauseNode = PolygonNode::allocWithTexture(_assets->get<Texture>("pause_button"));
	std::shared_ptr<cugl::Node> _playNode = PolygonNode::allocWithTexture(_assets->get<Texture>("play_button"));
	std::shared_ptr<cugl::Node> _restartNode = PolygonNode::allocWithTexture(_assets->get<Texture>("restart_button"));
	std::shared_ptr<cugl::Node> _quitNode = PolygonNode::allocWithTexture(_assets->get<Texture>("quit_button"));
	_gameOverScreen = PolygonNode::allocWithTexture(_assets->get<Texture>("game_over_screen"));
	_gameOverText = PolygonNode::allocWithTexture(_assets->get<Texture>("game_over"));

	float xMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxX();
	float yMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxY();

	_gameOverScreen->setVisible(false);
	_gameOverScreen->setPosition(0, 0);
	_gameOverText->setVisible(false);
	_gameOverText->setPosition(0, 75);

	_menuButton = cugl::Button::alloc(_backNode);
	_menuButton->setPosition(0, 10000);
	_menuButton->setVisible(false);

	_pauseButton = cugl::Button::alloc(_pauseNode);
	_pauseButton->setPosition(-xMax/2.0, -yMax/2.0);
	_pauseButton->setScale(.5, .5);

	_playButton = cugl::Button::alloc(_playNode);
	_playButton->setPosition(-200, 10000);
	_playButton->setVisible(false);

	_quitButton = cugl::Button::alloc(_quitNode);
	_quitButton->setPosition(-200, 10000);
	_quitButton->setVisible(false);

	_restartButton = cugl::Button::alloc(_restartNode);
	_restartButton->setPosition(-200, 10000);
	_restartButton->setVisible(false);

	_menuButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_didClickMenu = true;
		}
	});

	_pauseButton->setListener([=](const std::string& name, bool down) {
		if (!down) {
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

	_menuButton->activate(5);
	_pauseButton->activate(6);
	_playButton->activate(7);
	_quitButton->activate(8);
	_restartButton->activate(9);
	_uiNode->addChild(_menuButton, UNIT_PRIORITY);
	_uiNode->addChild(_pauseButton, UNIT_PRIORITY);
	_uiNode->addChild(_playButton, UNIT_PRIORITY);
	_uiNode->addChild(_gameOverScreen, UNIT_PRIORITY);
	_uiNode->addChild(_gameOverText, UNIT_PRIORITY);
	_uiNode->addChild(_quitButton, UNIT_PRIORITY);
	_uiNode->addChild(_restartButton, UNIT_PRIORITY);
	
	_worldnode->addChild(_uiNode, 2);
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
