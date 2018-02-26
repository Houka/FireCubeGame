//
//  RDRocketModel.cpp
//  Rocket Demo
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
//  Author: Walker White
//  Version: 1/10/17
//
#include "GameState.h"
#include "LevelConstants.h"
#include "TileModel.h"
#include "PlayerModel.h"
#include "RDEnemyModel.h"


using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

/** The number of frames for the afterburner */
#define FIRE_FRAMES     4

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f
#define DEFAULT_GRAVITY 0.0f

#pragma mark -
#pragma mark Constructors

/**
* Creates a new, empty level.
*/
GameState::GameState(void) {
	Asset();
}

/**
* Destroys this level, releasing all resources.
*/
GameState::~GameState(void) {
	unload();
	clearRootNode();
}


/**
* Disposes all resources and assets of this rocket
*
* Any assets owned by this object will be immediately released.  Once
* disposed, a rocket may not be used until it is initialized again.
*/
void GameState::dispose() {
	_world = nullptr;
}

void GameState::unload() { }

/**
* Clears the root scene graph node for this level
*/
void GameState::clearRootNode() { }

#pragma mark -
#pragma mark Asset Loading
/**
* Loads this game level from the source file
*
* This load method should NEVER access the AssetManager.  Assets are loaded in
* parallel, not in sequence.  If an asset (like a game level) has references to
* other assets, then these should be connected later, during scene initialization.
*
* @return true if successfully loaded the asset from a file
*/
bool GameState::preload(const std::string& file) {
	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
	return preload(reader->readJson());
}

/**
* Loads this game level from the source file
*
* This load method should NEVER access the AssetManager.  Assets are loaded in
* parallel, not in sequence.  If an asset (like a game level) has references to
* other assets, then these should be connected later, during scene initialization.
*
* @return true if successfully loaded the asset from a file
*/
bool GameState::preload(const std::shared_ptr<cugl::JsonValue>& json) {
	if (json == nullptr) {
		CUAssertLog(false, "Failed to load level file");
		return false;
	}
	// Initial geometry
	auto canvas = json->get(CANVAS_FIELD);
	int canvasW = canvas->get(WIDTH_FIELD)->asInt();
	int canvasH = canvas->get(HEIGHT_FIELD)->asInt();
	_canvasDim.set(canvasW, canvasH);

	auto tilesets = json->get(TILESETS_FIELD);
	auto protoTileset = tilesets.get()->get(0);
	int tileW = protoTileset->get(TILE_WIDTH_FIELD)->asInt();
	int tileH = protoTileset->get(TILE_HEIGHT_FIELD)->asInt();
	_tileDim.set(tileW, tileH);

	int worldW = canvasW / tileW;
	int worldH = canvasH / tileH;
	_worldDim.size.set(worldW, worldH);

	_physicsWorld = cugl::ObstacleWorld::alloc(_worldDim, cugl::Vec2(0,DEFAULT_GRAVITY));
	_physicsWorld->setGravity(cugl::Vec2(0, 0));
	//CULog(_physicsWorld->getGravity().toString().c_str());
	// Create the arena
	if (!loadWorld(json, worldW, worldH)) {
		CUAssertLog(false, "Failed to load world");
		return false;
	}

	// Create the player and enemy(s)
	if (!loadEntities(json)) {
		CUAssertLog(false, "Failed to load player");
		return false;
	}


	return true;
}

bool GameState::loadWorld(const std::shared_ptr<JsonValue>& json, int worldW, int worldH) {
	bool success = false;
	auto protoWorldLayer = json->get(LAYERS_FIELD)->get(0);
	if (protoWorldLayer != nullptr) {
		success = true;

		auto worldData = protoWorldLayer->get(DATA_FIELD)->asFloatArray();
		_world = new TILE_TYPE*[worldH];
		for (int i = 0; i < worldH; i++) {
			_world[i] = new TILE_TYPE[worldW];
		}

		int count = 0;
		for (int i = worldH-1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int tileVal = worldData[count]*10;

				TILE_TYPE tileType;
				switch (tileVal) {
					case 0:
						tileType = TILE_TYPE::WATER;
						break;
					case 1:
						tileType = TILE_TYPE::ISLAND;
						break;
					case 2:
						tileType = TILE_TYPE::ISLAND_BASE;
						break;
					case 10:
						tileType = TILE_TYPE::NW_LAND;
						break;
					case 11:
						tileType = TILE_TYPE::W_LAND;
						break;
					case 12:
						tileType = TILE_TYPE::SW_LAND;
						break;
					case 13:
						tileType = TILE_TYPE::L_LAND_BASE;
						break;
					case 20:
						tileType = TILE_TYPE::N_LAND;
						break;
					case 21:
						tileType = TILE_TYPE::LAND;
						break;
					case 22:
						tileType = TILE_TYPE::S_LAND;
						break;
					case 23:
						tileType = TILE_TYPE::LAND_BASE;
						break;
					case 30:
						tileType = TILE_TYPE::NE_LAND;
						break;
					case 31:
						tileType = TILE_TYPE::E_LAND;
						break;
					case 32:
						tileType = TILE_TYPE::SE_LAND;
						break;
					case 33:
						tileType = TILE_TYPE::R_LAND_BASE;
						break;
					default:
						CUAssertLog(false, "Invalid tile value.");
				}
				_world[i][j] = tileType;
				loadTile(Vec2(j, i), _tileDim, tileType);

				count++;
			}
		}
	}
	return success;
}


bool GameState::loadTile(Vec2 tilePos, Size tileDim, TILE_TYPE tileType) {
	bool success = true;

	// Get the object, which is automatically retained
	std::shared_ptr<TileModel> tile = TileModel::alloc(tilePos, tileDim);
	
	// Using the key makes too many sounds
	// crate->setName(reader.getKey());
	switch (tileType) {
		case TILE_TYPE::WATER:
			tile->setTextureKey("water");
			tile->setName("water");
			break;
		case TILE_TYPE::ISLAND_BASE:
			tile->setTextureKey("island_base");
			tile->setName("island_base");
			break;
		case TILE_TYPE::ISLAND:
			tile->setTextureKey("island");
			tile->setName("island");
			break;
		case TILE_TYPE::NW_LAND:
		case TILE_TYPE::W_LAND:
		case TILE_TYPE::SW_LAND:
		case TILE_TYPE::N_LAND:
		case TILE_TYPE::LAND:
		case TILE_TYPE::S_LAND:
		case TILE_TYPE::NE_LAND:
		case TILE_TYPE::E_LAND:
		case TILE_TYPE::SE_LAND:
			tile->setTextureKey("land");
			tile->setName("land");
			break;
		case TILE_TYPE::L_LAND_BASE:
			tile->setTextureKey("l_land_base");
			tile->setName("l_land_base");
			break;
		case TILE_TYPE::LAND_BASE:
			tile->setTextureKey("land_base");
			tile->setName("land_base");
			break;
		case TILE_TYPE::R_LAND_BASE:
			tile->setTextureKey("r_land_base");
			tile->setName("r_land_base");
			break;
		default:
			CUAssertLog(false, "Invalid tile type.");
			break;
	}

	tile->setBodyType(b2_staticBody);
/*
	crate->setDensity(json->getDouble(DENSITY_FIELD));
	crate->setFriction(json->getDouble(FRICTION_FIELD));
	crate->setRestitution(json->getDouble(RESTITUTION_FIELD));
	crate->setAngularDamping(json->getDouble(DAMPING_FIELD));
	crate->setAngleSnap(0);*/     // Snap to the nearest degree

								// Set the texture value
	_tiles.push_back(tile);

	return success;
}

bool GameState::loadEntities(const std::shared_ptr<cugl::JsonValue>& json) {
	bool success = false;
	auto protoObjectsLayer = json->get(LAYERS_FIELD)->get(1);
	if (protoObjectsLayer != nullptr) {
		success = true;

		auto objectsData = protoObjectsLayer->get(DATA_FIELD)->asFloatArray();

		cugl::Vec2 playerPos;
		cugl::Vec2 enemyPos;
		int count = 0;
		for (int i = _worldDim.size.getIHeight() - 1; i >= 0; i--) {
			for (int j = 0; j < _worldDim.size.getIWidth(); j++) {
				int objVal = objectsData[count] * 10;
				if (objVal == 40) {
					playerPos = cugl::Vec2(j,i);
				}
				else if (objVal == 41) {
					enemyPos = cugl::Vec2(j,i);
				}
				count++;
			}
		}

		_player = PlayerModel::alloc(playerPos, _tileDim);
		_player->setDrawScale(_scale.x);
		_player->setName("player");
		_player->setTextureKey("player");

		_player->setBodyType(b2_dynamicBody);

		_enemy = EnemyModel::alloc(enemyPos, _tileDim);
		_enemy->setDrawScale(_scale.x);
		_enemy->setName("enemy");
		_enemy->setTextureKey("enemy");


		/*_enemy = EnemyModel::alloc(enemyPos, _tileDim);
		_enemy->setDrawScale(_scale.x);
		_enemy->setName("enemy");
		_enemy->setTextureKey("enemy");*/
		
		if (success) {
			_physicsWorld->addObstacle(_player);
			_physicsWorld->addObstacle(_enemy);
		}
		else {
			CUAssertLog(false, "Failed to add player object.");
			_player = nullptr;
			_enemy = nullptr;
		}
	}
	else {
		CUAssertLog(false, "Failed to load objects layer.");
	}
	return success;
}


void GameState::setRootNode(const std::shared_ptr<Node>& node) {
	if (_root != nullptr) {
		clearRootNode();
	}

	_root = node;
	_scale.set(_canvasDim.getIWidth() / _worldDim.size.width,
		_canvasDim.getIHeight() / _worldDim.size.height);

	// Create, but transfer ownership to root
	_worldnode = Node::alloc();
	_worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_worldnode->setPosition(Vec2::ZERO);

	_debugnode = Node::alloc();
	_debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
	_debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_debugnode->setPosition(Vec2::ZERO);

	_root->addChild(_worldnode, 0);
	_root->addChild(_debugnode, 1);

	// Add the individual elements
	std::shared_ptr<PolygonNode> poly;
	std::shared_ptr<WireNode> draw;

	for (auto it = _tiles.begin(); it != _tiles.end(); ++it) {
		std::shared_ptr<TileModel> tile = *it;
		std::shared_ptr<PolygonNode> sprite = PolygonNode::allocWithTexture(_assets->get<Texture>(tile->getTextureKey()));
		int indx = (std::rand() % 2 == 0 ? 2 : 1);
		addObstacle(tile, sprite, TILE_PRIORITY + indx);   // PUT SAME TEXTURES IN SAME LAYER!!!
	}

	if (_player != nullptr) {
		auto playerNode = PolygonNode::allocWithTexture(_assets->get<Texture>(_player->getTextureKey()));
		_player->setShipNode(playerNode);
		_player->setDrawScale(_scale.x);

		// Create the polygon node (empty, as the model will initialize)
		_worldnode->addChild(playerNode, ENTITY_PRIORITY);
		_player->setDebugScene(_debugnode);
	}

	if (_enemy != nullptr) {
		auto enemyNode = PolygonNode::allocWithTexture(_assets->get<Texture>(_enemy->getTextureKey()));
		_enemy->setShipNode(enemyNode);
		_enemy->setDrawScale(_scale.x);
		
		_worldnode->addChild(enemyNode, ENTITY_PRIORITY);
		_enemy->setDebugScene(_debugnode);
	}
}

/**
* Adds the physics object to the physics world and loosely couples it to the scene graph
*
* There are two ways to link a physics object to a scene graph node on the
* screen.  One way is to make a subclass of a physics object, like we did
* with rocket.  The other is to use callback functions to loosely couple
* the two.  This function is an example of the latter.
*
* In addition, scene graph nodes have a z-order.  This is the order they are
* drawn in the scene graph node.  Objects with the different textures should
* have different z-orders whenever possible.  This will cut down on the amount of drawing done
*
* param obj    The physics object to add
* param node   The scene graph node to attach it to
* param zOrder The drawing order
*/
void GameState::addObstacle(const std::shared_ptr<cugl::Obstacle>& obj,
	const std::shared_ptr<cugl::Node>& node,
	int zOrder) {
	node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	cugl::Vec2 pos = obj->getPosition();
	//CULog(pos.toString().c_str());
	cugl::Size x = _physicsWorld->getBounds().size;
	//CULog(x.toString().c_str());
	//_physicsWorld->addObstacle(obj);
	obj->setDebugScene(_debugnode);
	obj->setActive(false);
	// Position the scene graph node (enough for static objects)
	node->setPosition(obj->getPosition()*_scale);
	_worldnode->addChild(node, zOrder);

	// Dynamic objects need constant updating
	if (obj->getBodyType() == b2_dynamicBody) {
		Node* weak = node.get(); // No need for smart pointer in callback
		obj->setListener([=](Obstacle* obs) {
			weak->setPosition(obs->getPosition()*_scale);
			weak->setAngle(obs->getAngle());
		});
	}
}

