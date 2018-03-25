//
//  LevelController.cpp
//  Coalide
//
#include "LevelController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"
#include "ObjectModel.h"
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include <Box2D/Dynamics/b2World.h>

#include <string>

using namespace cugl;

#define NORMAL_MOTION .015

#pragma mark -
#pragma mark Constructors

/**
* Disposes all resources and assets of this game state.
*
* Any assets owned by this object will be immediately released.  Once
* disposed, a game state may not be used until it is initialized again.
*/
void LevelController::dispose() { }


#pragma mark -
#pragma mark Asset Loading
/**
* Loads this game level from the source file
*
* @return true if successfully loaded the asset from a file
*/
bool LevelController::preload(const std::string& file) {
	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
	return preload(reader->readJson());
}

/**
* Loads this game level from the source file
*
* @return true if successfully loaded the asset from the json
*/
bool LevelController::preload(const std::shared_ptr<JsonValue>& json) {
	if (json == nullptr) {
		CUAssertLog(false, "Failed to load level file");
		return false;
	}
	// Initial geometry
	auto canvas = json->get(CANVAS_FIELD);
	int canvasW = canvas->get(WIDTH_FIELD)->asInt();
	int canvasH = canvas->get(HEIGHT_FIELD)->asInt();

	auto tilesets = json->get(TILESETS_FIELD);
	auto protoTileset = tilesets.get()->get(0);
	int tileW = protoTileset->get(TILE_WIDTH_FIELD)->asInt();
	int tileH = protoTileset->get(TILE_HEIGHT_FIELD)->asInt();
	_tileDim.set(tileW, tileH);

	int worldW = canvasW / tileW;
	int worldH = canvasH / tileH;
	_bounds.size.set(worldW, worldH);

	_scale.set(canvasW/worldW-.5, canvasH/worldH-.5);

	_world = ObstacleWorld::alloc(_bounds, cugl::Vec2(0, DEFAULT_GRAVITY));
    
    //for time slow down
    _world->setLockStep(true);
    _world->setStepsize(NORMAL_MOTION);
    
	
	// Create the arena
	if (!loadTerrain(json)) {
		CUAssertLog(false, "Failed to load world");
		return false;
	}

	// Create the player and enemy(s)
	if (!loadUnits(json)) {
		CUAssertLog(false, "Failed to load player");
		return false;
	}

	addFrictionJoints();

	buildGameState();

	return true;
}

bool LevelController::loadTerrain(const std::shared_ptr<JsonValue>& json) {
	bool success = false;

	int worldW = _bounds.size.getIWidth();
	int worldH = _bounds.size.getIHeight();

	_terrain = BoxObstacle::alloc(Vec2(worldW/2,worldH/2), Size(worldW, worldH));
	_terrain->setName("terrain");
	_terrain->setBodyType(b2_staticBody);

	b2Filter filter;
	filter.categoryBits = CATEGORY_TERRAIN;
	filter.maskBits = NULL;
	filter.groupIndex = NULL;
	_terrain->setFilterData(filter);

	_world->addObstacle(_terrain);

	auto protoWaterLayer = json->get(LAYERS_FIELD)->get(2);
	auto protoWorldLayer = json->get(LAYERS_FIELD)->get(0);
	
	if (protoWorldLayer != nullptr) {
		success = true;

		auto waterData = protoWaterLayer->get(DATA_FIELD)->asFloatArray();
		auto worldData = protoWorldLayer->get(DATA_FIELD)->asFloatArray();
		_board = new float*[worldH];
		
		for (int i = 0; i < worldH; i++) {
			_board[i] = new float[worldW];
		}

		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int tileVal = worldData[count] * 10;
				int waterVal = waterData[count] * 10;

				TILE_TYPE tileType;
				switch (tileVal) {
				case -10:
					switch (waterVal) {
					case 0:
						tileType = TILE_TYPE::WATER;
						_board[i][j] = 0;
						break;
					case 2:
						tileType = TILE_TYPE::ISLAND_BASE;
						_board[i][j] = 0;
						break;
					case 13:
						tileType = TILE_TYPE::L_LAND_BASE;
						_board[i][j] = 0;
						break;
					case 23:
						tileType = TILE_TYPE::C_LAND_BASE;
						_board[i][j] = 0;
						break;
					case 33:
						tileType = TILE_TYPE::R_LAND_BASE;
						_board[i][j] = 0;
						break;
					default:
						CUAssertLog(false, "Invalid water tile value.");
					}
					break;
				case 1:
					tileType = TILE_TYPE::ISLAND;
					_board[i][j] = 10;
					break;
				case 10:
					tileType = TILE_TYPE::NW_LAND;
					_board[i][j] = 10;
					break;
				case 11:
					tileType = TILE_TYPE::W_LAND;
					_board[i][j] = 10;
					break;
				case 12:
					tileType = TILE_TYPE::SW_LAND;
					_board[i][j] = 10;
					break;
				case 20:
					tileType = TILE_TYPE::N_LAND;
					_board[i][j] = 10;
					break;
				case 21:
					tileType = TILE_TYPE::LAND;
					_board[i][j] = 10;
					break;
				case 22:
					tileType = TILE_TYPE::S_LAND;
					_board[i][j] = 10;
					break;
				case 30:
					tileType = TILE_TYPE::NE_LAND;
					_board[i][j] = 10;
					break;
				case 31:
					tileType = TILE_TYPE::E_LAND;
					_board[i][j] = 10;
					break;
				case 32:
					tileType = TILE_TYPE::SE_LAND;
					_board[i][j] = 10;
					break;
				case 42:
					tileType = TILE_TYPE::SAND;
					_board[i][j] = 20;
					break;
				case 43:
					tileType = TILE_TYPE::ICE;
					_board[i][j] = 2;
					break;
				default:
					CUAssertLog(false, "Invalid tile value.");
				}
				loadTile(Vec2(j+.5, i+.5), tileType);
				count++;
			}
		}
	}
	return success;
}

bool LevelController::loadTile(Vec2 tilePos, TILE_TYPE tileType) {
	bool success = true;

	std::shared_ptr<TileModel> tile = TileModel::alloc(tilePos, UNIT_DIM);

	switch (tileType) {
	case TILE_TYPE::WATER:
		tile->setTextureKey(PROTO_LEVEL_KEY WATER_TEXTURE);
		tile->setFriction(0);
		tile->setType(TILE_TYPE::WATER);
		break;
	case TILE_TYPE::ISLAND_BASE:
		tile->setTextureKey(PROTO_LEVEL_KEY ISLAND_BASE_TEXTURE);
		tile->setFriction(0);
		tile->setType(TILE_TYPE::WATER);
		break;
	case TILE_TYPE::ISLAND:
		tile->setTextureKey(PROTO_LEVEL_KEY ISLAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::NW_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY NW_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::W_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY W_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::SW_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY SW_LAND_TEXTURE);
		tile->setType(TILE_TYPE::LAND);
		tile->setFriction(10);
		break;
	case TILE_TYPE::N_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY N_LAND_TEXTURE);
		tile->setType(TILE_TYPE::LAND);
		tile->setFriction(10);
		break;
	case TILE_TYPE::LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::S_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY S_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::NE_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY NE_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::E_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY E_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::SE_LAND:
		tile->setTextureKey(PROTO_LEVEL_KEY SE_LAND_TEXTURE);
		tile->setFriction(10);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::L_LAND_BASE:
		tile->setTextureKey(PROTO_LEVEL_KEY L_LAND_BASE_TEXTURE);
		tile->setFriction(0);
		tile->setType(TILE_TYPE::WATER);
		break;
	case TILE_TYPE::C_LAND_BASE:
		tile->setTextureKey(PROTO_LEVEL_KEY C_LAND_BASE_TEXTURE);
		tile->setFriction(0);
		tile->setType(TILE_TYPE::WATER);
		break;
	case TILE_TYPE::R_LAND_BASE:
		tile->setTextureKey(PROTO_LEVEL_KEY R_LAND_BASE_TEXTURE);
		tile->setFriction(0);
		tile->setType(TILE_TYPE::WATER);
		break;
	case TILE_TYPE::SAND:
		tile->setTextureKey(PROTO_LEVEL_KEY SAND_TEXTURE);
		tile->setFriction(20);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::ICE:
		tile->setTextureKey(PROTO_LEVEL_KEY ICE_TEXTURE);
		tile->setFriction(3);
		tile->setType(TILE_TYPE::LAND);
		break;
	default:
		CUAssertLog(false, "Invalid tile type.");
		break;
	}

	//_world->addObstacle(tile);
	
	_tiles.push_back(tile);

	return success;
}

bool LevelController::loadUnits(const std::shared_ptr<cugl::JsonValue>& json) {
	bool success = false;
	auto objectsLayer = json->get(LAYERS_FIELD)->get(1);
	if (objectsLayer != nullptr) {
		success = true;

		int worldW = _bounds.size.getIWidth();
		int worldH = _bounds.size.getIHeight();

		auto objectsData = objectsLayer->get(DATA_FIELD)->asFloatArray();

		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int objVal = objectsData[count] * 10;

				if (objVal == 40) {
					_player = PlayerModel::alloc(Vec2(j+.5,i+.5), UNIT_DIM);
					_world->addObstacle(_player);
				}
				else if (objVal == 41) {
					std::shared_ptr<EnemyModel> enemy = EnemyModel::alloc(Vec2(j+.5,i+.5), UNIT_DIM);
					enemy->setTextureKey(PROTO_LEVEL_KEY ENEMY_NAME);

					_world->addObstacle(enemy);
					_enemies.push_back(enemy);
				}
				else if (objVal == 50) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(IMMOBILE_NAME);
					object->setTextureKey(PROTO_LEVEL_KEY IMMOBILE_NAME);
					object->setBodyType(b2_staticBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}
				else if (objVal == 51) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(MOVABLE_NAME);
					object->setTextureKey(PROTO_LEVEL_KEY MOVABLE_NAME);
					object->setBodyType(b2_dynamicBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}
				else if (objVal == 52) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(BREAKABLE_NAME); 
					object->setTextureKey(PROTO_LEVEL_KEY BREAKABLE_NAME);
					object->setBodyType(b2_staticBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}
				count++;
			}
		}
	}
	else {
		CUAssertLog(false, "Failed to load objects layer.");
	}
	return success;
}

void LevelController::addFrictionJoints() {
	b2FrictionJointDef jointDef;

	jointDef.localAnchorA.SetZero();
	jointDef.localAnchorB.SetZero();

	jointDef.bodyB = _terrain->getBody();
	jointDef.maxForce = 10;
	jointDef.maxTorque = 10;

	jointDef.bodyA = _player->getBody();
	_player->setFrictionJoint((b2FrictionJoint*)(_world->getWorld()->CreateJoint(&jointDef)));

	for (int i = 0; i < _enemies.size(); i++) {
		jointDef.maxForce = 0;
		jointDef.maxTorque = 0;

		jointDef.bodyA = _enemies[i]->getBody();
		_enemies[i]->setFrictionJoint((b2FrictionJoint*)(_world->getWorld()->CreateJoint(&jointDef)));
	}

	for (int i = 0; i < _objects.size(); i++) {
		jointDef.maxForce = 0;
		jointDef.maxTorque = 0;

		jointDef.bodyA = _objects[i]->getBody();
		_objects[i]->setFrictionJoint((b2FrictionJoint*)(_world->getWorld()->CreateJoint(&jointDef)));
	}
}

void LevelController::buildGameState() {
	_gamestate = GameState::alloc();

	_gamestate->setPlayer(_player);
	_gamestate->setEnemies(_enemies);
	_gamestate->setObjects(_objects);
	_gamestate->setTiles(_tiles);
	_gamestate->setWorld(_world);
	_gamestate->setBoard(_board);
	_gamestate->setDrawScale(_scale);

	_levelBuilt = true;
}

/**
* Unloads this game level, releasing all sources
*/
void LevelController::unload() {
	if (_player != nullptr) {
		if (_world != nullptr) {
			_gamestate->getWorld()->removeObstacle(_player.get());
		}
		_player = nullptr;
	}

	for (auto it = _enemies.begin(); it != _enemies.end(); ++it) {
		if (_world != nullptr) {
			if (!(*it).get()->isRemoved()) {
				_gamestate->getWorld()->removeObstacle((*it).get());
			}
		}
		(*it) = nullptr;
	}
	_enemies.clear();

	for (auto it = _objects.begin(); it != _objects.end(); ++it) {
		if (_world != nullptr) {
			if (!(*it).get()->isRemoved()) {
				_gamestate->getWorld()->removeObstacle((*it).get());
			}	
		}
		(*it) = nullptr;
	}
	_objects.clear();

	//for (auto it = _tiles.begin(); it != _tiles.end(); ++it) {
	//	if (_world != nullptr) {
	//		_gamestate->getWorld()->removeObstacle((*it).get());
	//	}
	//	(*it) = nullptr;
	//}
	_tiles.clear();

	if (_world != nullptr) {
		_world->clear();
		_world = nullptr;
	}
}
