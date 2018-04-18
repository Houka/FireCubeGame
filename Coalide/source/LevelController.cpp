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
#include "Constants.h"
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include <Box2D/Dynamics/b2World.h>

#include <string>

using namespace cugl;


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

	int tileW = tilesets->get(0)->get(TILE_WIDTH_FIELD)->asInt();
	int tileH = tilesets->get(0)->get(TILE_HEIGHT_FIELD)->asInt();
	_tileDim.set(tileW, tileH);

	int worldTilesetW = tilesets->get(0)->get(IMAGE_WIDTH_FIELD)->asInt();
	int worldTilesetH = tilesets->get(0)->get(IMAGE_HEIGHT_FIELD)->asInt();
	_worldTilesetDim.set(worldTilesetW, worldTilesetH);

	int waterTilesetW = tilesets->get(1)->get(IMAGE_WIDTH_FIELD)->asInt();
	int waterTilesetH = tilesets->get(1)->get(IMAGE_HEIGHT_FIELD)->asInt();
	_waterTilesetDim.set(waterTilesetW, waterTilesetH);

	int worldW = canvasW / tileW;
	int worldH = canvasH / tileH;
	_bounds.size.set(worldW, worldH);

	_scale.set(canvasW/worldW-1, canvasH/worldH-1);

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

	auto waterLayer = json->get(LAYERS_FIELD)->get(0);
	auto worldLayer = json->get(LAYERS_FIELD)->get(1);

	if (worldLayer != nullptr) {
		success = true;

		auto waterData = waterLayer->get(DATA_FIELD)->asFloatArray();
		auto worldData = worldLayer->get(DATA_FIELD)->asFloatArray();
		_board = new int*[worldH];
		_tileBoard = new std::shared_ptr<TileModel>*[worldH];

		for (int i = 0; i < worldH; i++) {
			_board[i] = new int[worldW];
			_tileBoard[i] = new std::shared_ptr<TileModel>[worldW];
		}

		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				loadWaterTile(Vec2(j + .5, i + .5), waterData[count], TILE_TYPE::WATER, waterLayer);

				float tileVal = worldData[count];

				if (std::ceil(tileVal) - tileVal < .01) {
					tileVal = std::ceil(tileVal);
				}

				int tileColumn = std::floor(tileVal);
				TILE_TYPE tileType;

				switch (tileColumn) {
				case -1:
					_board[i][j] = 0;
					break;
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					_board[i][j] = 10;
					loadLandTile(Vec2(j + .5, i + .5), tileVal, TILE_TYPE::GRASS, worldLayer);
					break;
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
					_board[i][j] = 2;
					loadLandTile(Vec2(j + .5, i + .5), tileVal, TILE_TYPE::ICE, worldLayer);
					break;
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
					_board[i][j] = 20;
					loadLandTile(Vec2(j + .5, i + .5), tileVal, TILE_TYPE::SAND, worldLayer);
					break;
				default:
					CUAssertLog(false, "Invalid tile data.");
					break;
				}
				count++;
			}
		}
	}
	return success;
}

bool LevelController::loadLandTile(Vec2 tilePos, float tileVal, TILE_TYPE tileType, std::shared_ptr<JsonValue>& layer) {
	bool success = true;

	std::shared_ptr<TileModel> tile = TileModel::alloc(tilePos, UNIT_DIM);

	tile->setType(tileType);
	int col = std::floor(tileVal);
	float rowFloat = tileVal - col;
	int row;
	if (std::ceil(rowFloat*10) - rowFloat*10 < .05) {
		row = std::ceil(rowFloat*10);
	}
	else if (rowFloat*10 - std::floor(rowFloat*10) < .05) {
		row = rowFloat * 10;
	}
	else {
		if (std::ceil(rowFloat * 100) - rowFloat * 100 < .05) {
			row = std::ceil(rowFloat * 100);
		}
		else {
			row = rowFloat * 100;
		}
	}

	double x0 = col*_tileDim.width / _worldTilesetDim.width;
	double x1 = (col + 1)*_tileDim.width / _worldTilesetDim.width;
	double y0 = row*_tileDim.height / _worldTilesetDim.height;
	double y1 = (row + 1)*_tileDim.height / _worldTilesetDim.height;

	tile->setTextureKey(layer->get(TILESET_FIELD)->asString());
	tile->setSubTexture(x0, x1, y0, y1);

	_tiles.push_back(tile);
	_tileBoard[(int)tilePos.y][(int)tilePos.x] = tile;

	return success;
}

bool LevelController::loadWaterTile(Vec2 tilePos, float tileVal, TILE_TYPE tileType, std::shared_ptr<JsonValue>& layer) {
	bool success = true;

	std::shared_ptr<TileModel> tile = TileModel::alloc(tilePos, UNIT_DIM);

	tile->setType(tileType);
	int col = std::floor(tileVal);
	float rowFloat = tileVal - col;
	int row;
	if (std::ceil(rowFloat * 10) - rowFloat * 10 < .05) {
		row = std::ceil(rowFloat * 10);
	}
	else if (rowFloat * 10 - std::floor(rowFloat * 10) < .05) {
		row = rowFloat * 10;
	}
	else {
		if (std::ceil(rowFloat * 100) - rowFloat * 100 < .05) {
			row = std::ceil(rowFloat * 100);
		}
		else {
			row = rowFloat * 100;
		}
	}

	double x0 = col*_tileDim.width / _waterTilesetDim.width;
	double x1 = (col + 1)*_tileDim.width / _waterTilesetDim.width;
	double y0 = row*_tileDim.height / _waterTilesetDim.height;
	double y1 = (row + 1)*_tileDim.height / _waterTilesetDim.height;
	
	tile->setTextureKey(layer->get(TILESET_FIELD)->asString());
	tile->setSubTexture(x0, x1, y0, y1);
	
	_tiles.push_back(tile);
	_tileBoard[(int)tilePos.y][(int)tilePos.x] = tile;

	return success;
}

bool LevelController::loadUnits(const std::shared_ptr<cugl::JsonValue>& json) {
	bool success = false;
	auto objectsLayer = json->get(LAYERS_FIELD)->get(2);
	if (objectsLayer != nullptr) {
		success = true;

		int worldW = _bounds.size.getIWidth();
		int worldH = _bounds.size.getIHeight();

		auto objectsData = objectsLayer->get(DATA_FIELD)->asFloatArray();

		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int objVal = objectsData[count];
				
				std::shared_ptr<EnemyModel> enemy;
				std::shared_ptr<ObjectModel> object;

				b2Filter filter;

				switch (objVal) {
				case -1:
					break;
				case 0:
					_player = PlayerModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);

					_world->addObstacle(_player);
					break;
				case 1:
					enemy = EnemyModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					enemy->setTextureKey(ACORN);

					_world->addObstacle(enemy);
					_enemies.push_back(enemy);
					break;
				case 2:
					enemy = EnemyModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					enemy->setTextureKey(MUSHROOM);
					enemy->setMushroom();

					filter.categoryBits = CATEGORY_MUSHROOM;
					filter.maskBits = ~CATEGORY_SPORE;
					filter.groupIndex = NULL;
					enemy->setFilterData(filter);

					_world->addObstacle(enemy);
					_enemies.push_back(enemy);
					break;
				case 3:
					enemy = EnemyModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					enemy->setTextureKey(ONION);
					enemy->setOnion();
					enemy->setDensity(3);

					_world->addObstacle(enemy);
					_enemies.push_back(enemy);
					break;
				case 4:
					object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setTextureKey(IMMOBILE_NAME);

					object->setName(IMMOBILE_NAME);
					object->setBodyType(b2_staticBody);

					_world->addObstacle(object);
					_objects.push_back(object);

					break;
				case 5:
					object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setTextureKey(MOVABLE_NAME);

					object->setName(MOVABLE_NAME);
					object->setBodyType(b2_dynamicBody);

					_world->addObstacle(object);
					_objects.push_back(object);
					break;
				default:
					CUAssertLog(false, "Invalid object data.");
					break;
				}
				
				/*else if (objVal == 50) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(IMMOBILE_NAME);
					object->setTextureKey(IMMOBILE_NAME);
					object->setBodyType(b2_staticBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}
				else if (objVal == 51) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(MOVABLE_NAME);
					object->setTextureKey(MOVABLE_NAME);
					object->setBodyType(b2_dynamicBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}
				else if (objVal == 52) {
					std::shared_ptr<ObjectModel> object = ObjectModel::alloc(Vec2(j + .5, i + .5), UNIT_DIM);
					object->setName(BREAKABLE_NAME); 
					object->setTextureKey(BREAKABLE_NAME);
					object->setBodyType(b2_staticBody);

					_world->addObstacle(object);
					_objects.push_back(object);
				}*/
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
	_gamestate->setTileBoard(_tileBoard);
	_gamestate->setDrawScale(_scale);
	_gamestate->setAssets(_assets);

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

	for (int i = 0; i < _enemies.size(); i++) {
		if (_world != nullptr) {
			if (!_enemies[i]->isRemoved()) {
				_gamestate->getWorld()->removeObstacle(_enemies[i].get());
			}
		}
		_enemies[i] = nullptr;
	}
	_enemies.clear();

	for (int i = 0; i < _objects.size(); i++) {
		if (_world != nullptr) {
			if (!_objects[i]->isRemoved()) {
				_gamestate->getWorld()->removeObstacle(_objects[i].get());
			}	
		}
		_objects[i] = nullptr;
	}
	_objects.clear();

	for (int i = 0; i < _bounds.size.getIHeight(); i++) {
		delete[] _board[i];
		delete[] _tileBoard[i];
	}

	delete[] _board;
	delete[] _tileBoard;
	

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
