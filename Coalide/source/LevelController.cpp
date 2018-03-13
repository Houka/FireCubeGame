//
//  LevelController.cpp
//  Coalide
//
#include "LevelController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"

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

	_scale.set(canvasW/worldW, canvasH/worldH);

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

	buildGameState();

	return true;
}

bool LevelController::loadTerrain(const std::shared_ptr<JsonValue>& json) {
	bool success = false;
	auto protoWorldLayer = json->get(LAYERS_FIELD)->get(0);
	if (protoWorldLayer != nullptr) {
		success = true;

		int worldW = _bounds.size.getIWidth();
		int worldH = _bounds.size.getIHeight();

		auto worldData = protoWorldLayer->get(DATA_FIELD)->asFloatArray();
		_board = new TILE_TYPE*[worldH];
		
		for (int i = 0; i < worldH; i++) {
			_board[i] = new TILE_TYPE[worldW];
		}

		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int tileVal = worldData[count] * 10;

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
				_board[i][j] = tileType;
				loadTile(Vec2(j, i), tileType);
				count++;
			}
		}
	}
	return success;
}

bool LevelController::loadTile(Vec2 tilePos, TILE_TYPE tileType) {
	bool success = true;

	// Get the object, which is automatically retained
	std::shared_ptr<TileModel> tile = TileModel::alloc(tilePos, UNIT_DIM);

	// Using the key makes too many sounds
	// crate->setName(reader.getKey());
	switch (tileType) {
	case TILE_TYPE::WATER:
		tile->setTextureKey("protosheet_water");
		break;
	case TILE_TYPE::ISLAND_BASE:
		tile->setTextureKey("protosheet_islandBase");
		break;
	case TILE_TYPE::ISLAND:
		tile->setTextureKey("protosheet_island");
		break;
	case TILE_TYPE::NW_LAND:
		tile->setTextureKey("protosheet_nwLand");
		break;
	case TILE_TYPE::W_LAND:
		tile->setTextureKey("protosheet_wLand");
		break;
	case TILE_TYPE::SW_LAND:
		tile->setTextureKey("protosheet_swLand");
		break;
	case TILE_TYPE::N_LAND:
		tile->setTextureKey("protosheet_nLand");
		break;
	case TILE_TYPE::LAND:
		tile->setTextureKey("protosheet_land");
		break;
	case TILE_TYPE::S_LAND:
		tile->setTextureKey("protosheet_sLand");
		break;
	case TILE_TYPE::NE_LAND:
		tile->setTextureKey("protosheet_neLand");
		break;
	case TILE_TYPE::E_LAND:
		tile->setTextureKey("protosheet_eLand");
		break;
	case TILE_TYPE::SE_LAND:
		tile->setTextureKey("protosheet_seLand");
		break;
	case TILE_TYPE::L_LAND_BASE:
		tile->setTextureKey("protosheet_lLandBase");
		break;
	case TILE_TYPE::LAND_BASE:
		tile->setTextureKey("protosheet_cLandBase");
		break;
	case TILE_TYPE::R_LAND_BASE:
		tile->setTextureKey("protosheet_rLandBase");
		break;
	default:
		CUAssertLog(false, "Invalid tile type.");
		break;
	}

	tile->setType(tileType);
	tile->setBodyType(b2_staticBody);
	tile->setDrawScale(_scale.x);
	//_world->addObstacle(tile);
	
	_tiles.push_back(tile);

	return success;
}

bool LevelController::loadUnits(const std::shared_ptr<cugl::JsonValue>& json) {
	bool success = false;
	auto protoObjectsLayer = json->get(LAYERS_FIELD)->get(1);
	if (protoObjectsLayer != nullptr) {
		success = true;

		int worldW = _bounds.size.getIWidth();
		int worldH = _bounds.size.getIHeight();

		auto objectsData = protoObjectsLayer->get(DATA_FIELD)->asFloatArray();

		int enemyN = 0;
		int count = 0;
		for (int i = worldH - 1; i >= 0; i--) {
			for (int j = 0; j < worldW; j++) {
				int objVal = objectsData[count] * 10;
				if (objVal == 40) {
					_player = PlayerModel::alloc(Vec2(j,i), UNIT_DIM);
					_player->setDrawScale(_scale.x);

					_world->addObstacle(_player);
				}
				else if (objVal == 41) {
					std::shared_ptr<EnemyModel> enemy = EnemyModel::alloc(Vec2(j,i), UNIT_DIM);
					enemy->setDrawScale(_scale.x);
					enemy->setName("enemy" + enemyN);
					enemy->setTextureKey("protosheet_enemy");
					enemy->setBodyType(b2_dynamicBody);

					_world->addObstacle(enemy);

					_enemies.push_back(enemy);

					enemyN++;
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

void LevelController::buildGameState() {
	_gamestate = GameState::alloc();

	_gamestate->setPlayer(_player);
	_gamestate->setEnemies(_enemies);
	_gamestate->setTiles(_tiles);
	_gamestate->setWorld(_world);
	_gamestate->setBoard(_board);
	_gamestate->setDrawScale(_scale);

	_levelBuilt = true;
}

/**
* Unloads this game level, releasing all sources
*/
void LevelController::unload() { }
