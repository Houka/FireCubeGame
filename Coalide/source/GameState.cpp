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
        auto playerNode = PolygonNode::allocWithTexture(assets->get<Texture>(_player->getTextureKey()));
        auto standingPlayerNode_f = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_f"));
        auto standingPlayerNode_fls = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_fls"));
        auto standingPlayerNode_l = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_l"));
        auto standingPlayerNode_bls = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_bls"));
        auto standingPlayerNode_b = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_b"));
        auto standingPlayerNode_brs = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_brs"));
        auto standingPlayerNode_r = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_r"));
        auto standingPlayerNode_frs = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_nicoal_frs"));
        
        auto chargingPlayerNode_f = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_f"));
        auto chargingPlayerNode_fls = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_fls"));
        auto chargingPlayerNode_l = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_l"));
        auto chargingPlayerNode_bls = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_bls"));
        auto chargingPlayerNode_b = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_b"));
        auto chargingPlayerNode_brs = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_brs"));
        auto chargingPlayerNode_r = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_r"));
        auto chargingPlayerNode_frs = PolygonNode::allocWithTexture(assets->get<Texture>("nicoal_charging_frs"));
        
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
        
        //        chargingPlayerNode->setVisible(false);
        //        _player->setStandingNode(playerNode);
        //        _player->setChargingNode(chargingPlayerNode);
        //        _player->setAnimationNode(animationNode);
        _player->setDrawScale(_scale.x);
        _player->setDebugScene(_debugnode);
        
        // create the aim arrow
        const std::vector<cugl::Vec2> arrowLine = { cugl::Vec2(0,0), cugl::Vec2(0, 2) };
        std::shared_ptr<cugl::PathNode> _arrow = PathNode::allocWithVertices(arrowLine, 1, cugl::PathJoint::NONE, cugl::PathCap::NONE, false);
        _arrow->setAnchor(cugl::Vec2(0.0, 0.0));
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
        //        _worldnode->addChild(chargingPlayerNode, UNIT_PRIORITY);
    }
    
    if (_enemies.size() > 0) {
        for (auto it = _enemies.begin(); it != _enemies.end(); ++it) {
            std::shared_ptr<EnemyModel> enemy = *it;
            auto enemyNode = PolygonNode::allocWithTexture(assets->get<Texture>(enemy->getTextureKey()));
            enemy->setNode(enemyNode);
            enemy->setDrawScale(_scale.x);
            //enemy->setDebugScene(_debugnode);
            
            _worldnode->addChild(enemyNode, UNIT_PRIORITY);
        }
    }
    
    if (_objects.size() > 0) {
        for (auto it = _objects.begin(); it != _objects.end(); ++it) {
            std::shared_ptr<ObjectModel> object = *it;
            auto objectNode = PolygonNode::allocWithTexture(assets->get<Texture>(object->getTextureKey()));
            object->setNode(objectNode);
            object->setDrawScale(_scale.x);
            //object->setDebugScene(_debugnode);
            
            _worldnode->addChild(objectNode, UNIT_PRIORITY);
        }
    }
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
