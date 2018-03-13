//
//	GameState.cpp
//	Coalide
//
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"

#include <string>

using namespace cugl;


/**
* Clears the root scene graph node for this level
*/
void GameState::clearRootNode() { }

/**
* Destroys this level, releasing all resources.
*/
void GameState::dispose() { }

/**
* Sets the scene graph node for drawing purposes.
*
* @param value  the scene graph node for drawing purposes.
*
* @retain  a reference to this scene graph node
* @release the previous scene graph node used by this object
*/
void GameState::setRootNode(const std::shared_ptr<Node>& node, std::shared_ptr<AssetManager> assets) {
	if (_rootnode != nullptr) {
		clearRootNode();
	}

	_rootnode = node;

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
	if (_tiles.size() > 0) {
		for (auto it = _tiles.begin(); it != _tiles.end(); ++it) {
			std::shared_ptr<TileModel> tile = *it;
			auto tileNode = PolygonNode::allocWithTexture(assets->get<Texture>(tile->getTextureKey()));
			tile->setNode(tileNode);
			tile->setDrawScale(_scale.x);
			tile->setDebugScene(_debugnode);

			tileNode->setPosition(tile->getPosition()*_scale);

			_worldnode->addChild(tileNode, TILE_PRIORITY);
		}
	}

	if (_player != nullptr) {
		auto playerNode = PolygonNode::allocWithTexture(assets->get<Texture>(_player->getTextureKey()));
		_player->setNode(playerNode);
		_player->setDrawScale(_scale.x);
		_player->setDebugScene(_debugnode);

		// Create the polygon node (empty, as the model will initialize)
		_worldnode->addChild(playerNode, UNIT_PRIORITY);
	}

	if (_enemies.size() > 0) {
		for (auto it = _enemies.begin(); it != _enemies.end(); ++it) {
			std::shared_ptr<EnemyModel> enemy = *it;
			auto enemyNode = PolygonNode::allocWithTexture(assets->get<Texture>(enemy->getTextureKey()));
			enemy->setNode(enemyNode);
			enemy->setDrawScale(_scale.x);
			enemy->setDebugScene(_debugnode);

			_worldnode->addChild(enemyNode, UNIT_PRIORITY);
		}
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
