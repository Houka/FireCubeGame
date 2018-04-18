//
//	GameState.cpp
//	Coalide
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

	// Add the individual elements
	for (int i = 0; i < _tiles.size(); i++) {
		std::shared_ptr<TileModel> tile = _tiles[i];
		double* tileSubtexture = tile->getSubTexture();
		auto tileNode = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getTextureKey())->getSubTexture(tileSubtexture[0], tileSubtexture[1], tileSubtexture[2], tileSubtexture[3]));
		tile->setNode(tileNode);
		tile->setDrawScale(_scale.x);
		//tile->setDebugScene(_debugnode);

		tileNode->setPosition(tile->getPosition()*_scale);

		_worldnode->addChild(tileNode, TILE_PRIORITY);
	}

	if (_player != nullptr) {
		auto playerNode = PolygonNode::allocWithTexture(_assets->get<Texture>(_player->getTextureKey()));
		auto chargingPlayerNode = PolygonNode::allocWithTexture(_assets->get<Texture>("nicoal_charging"));
		_player->setNode(playerNode);
		chargingPlayerNode->setVisible(false);
		_player->setStandingNode(playerNode);
		_player->setChargingNode(chargingPlayerNode);
		_player->setDrawScale(_scale.x);
		//_player->setDebugScene(_debugnode);

		// create the aim arrow
		const std::vector<cugl::Vec2> arrowLine = { cugl::Vec2(0,0), cugl::Vec2(0, 2) };
		std::shared_ptr<cugl::PathNode> _arrow = PathNode::allocWithVertices(arrowLine, 1, cugl::PathJoint::NONE, cugl::PathCap::NONE, false);
		_arrow->setAnchor(cugl::Vec2(0.0, 0.0));
		_arrow->setVisible(false);
		_player->setArrow(_arrow);

		// Create the polygon node (empty, as the model will initialize)
		playerNode->addChild(_arrow, UNIT_PRIORITY);
		_worldnode->addChild(playerNode, UNIT_PRIORITY);
		_worldnode->addChild(chargingPlayerNode, UNIT_PRIORITY);
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
			auto objectNode = PolygonNode::allocWithTexture(_assets->get<Texture>(object->getTextureKey()));
			object->setNode(objectNode);
			object->setDrawScale(_scale.x);
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

	float xMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxX();
	float yMax = _player->getNode()->getScene()->getCamera()->getViewport().getMaxY();

	//_startNode->setPosition(50., 50);
	_menuButton = cugl::Button::alloc(_backNode);
	_menuButton->setPosition(0, 10000);
	_menuButton->setVisible(false);


	_pauseButton = cugl::Button::alloc(_pauseNode);
	_pauseButton->setPosition(-xMax/2.0, -yMax/2.0);
	_pauseButton->setScale(.5, .5);


	_playButton = cugl::Button::alloc(_playNode);
	_playButton->setPosition(-200, 10000);
	_playButton->setVisible(false);

	_menuButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_didClickMenu = true;
		}
	});

	_pauseButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_isPaused = true;
			_playButton->setVisible(true);
			_playButton->setPosition(-200, -100);
			_menuButton->setVisible(true);
			_menuButton->setPosition(0, -100);
			_pauseButton->setVisible(false);
		}
	});

	_playButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_isPaused = false;
			_playButton->setVisible(false);
			_playButton->setPosition(-200, 10000);
			_menuButton->setVisible(false);
			_menuButton->setPosition(0, 10000);
			_pauseButton->setVisible(true);
		}
	});

	_menuButton->activate(5);
	_pauseButton->activate(6);
	_playButton->activate(7);
	_uiNode->addChild(_menuButton, UNIT_PRIORITY);
	_uiNode->addChild(_pauseButton, UNIT_PRIORITY);
	_uiNode->addChild(_playButton, UNIT_PRIORITY);
	_rootnode->addChild(_uiNode, 2);
}

void GameState::addSporeNode(std::shared_ptr<EnemyModel> spore) {
	auto sporeNode = PolygonNode::allocWithTexture(_assets->get<Texture>(spore->getTextureKey()));
	spore->setNode(sporeNode);
	spore->setDrawScale(_scale.x);
	//spore->setDebugScene(_debugnode);

	_worldnode->addChild(sporeNode, UNIT_PRIORITY);
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
