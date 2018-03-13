//
//  LevelController.cpp
//  Coalide
//
#include "LevelController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"
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
	auto protoTileset = tilesets.get()->get(0);
	int tileW = protoTileset->get(TILE_WIDTH_FIELD)->asInt();
	int tileH = protoTileset->get(TILE_HEIGHT_FIELD)->asInt();
	_tileDim.set(tileW, tileH);

	int worldW = canvasW / tileW;
	int worldH = canvasH / tileH;
	_bounds.size.set(worldW, worldH);

	_scale.set(canvasW/worldW, canvasH/worldH);

	_world = ObstacleWorld::alloc(_bounds, cugl::Vec2(0, DEFAULT_GRAVITY));
	
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

	// Add friction joints to simulate top-down friction
	addFrictionJoints();

	buildGameState();

	return true;
}

bool LevelController::loadTerrain(const std::shared_ptr<JsonValue>& json) {
	bool success = false;

	int worldW = _bounds.size.getIWidth();
	int worldH = _bounds.size.getIHeight();

	_terrain = BoxObstacle::alloc(Vec2(0,0), Size(worldW, worldH));
	_terrain->setName("terrain");
	_world->addObstacle(_terrain);

	auto protoWorldLayer = json->get(LAYERS_FIELD)->get(0);
	if (protoWorldLayer != nullptr) {
		success = true;

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
					tileType = TILE_TYPE::C_LAND_BASE;
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
				case 42:
					tileType = TILE_TYPE::SAND;
					break;
				case 43:
					tileType = TILE_TYPE::ICE;
					break;
				default:
					CUAssertLog(false, "Invalid tile value.");
				}
				_board[i][j] = tileType;
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
		tile->setFriction(50);
		tile->setType(TILE_TYPE::LAND);
		break;
	case TILE_TYPE::ICE:
		tile->setTextureKey(PROTO_LEVEL_KEY ICE_TEXTURE);
		tile->setFriction(2);
		tile->setType(TILE_TYPE::LAND);
		break;
	default:
		CUAssertLog(false, "Invalid tile type.");
		break;
	}

	tile->setDrawScale(_scale.x);
	_world->addObstacle(tile);
	
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
					_player->setDrawScale(_scale.x);
					
					_world->addObstacle(_player);
				}
				else if (objVal == 41) {
					std::shared_ptr<EnemyModel> enemy = EnemyModel::alloc(Vec2(j+.5,i+.5), UNIT_DIM);
					enemy->setTextureKey(PROTO_LEVEL_KEY ENEMY_NAME);
					enemy->setDrawScale(_scale.x);

					_world->addObstacle(enemy);
					_enemies.push_back(enemy);
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
	jointDef.maxForce = 0;
	jointDef.maxTorque = 0;

	jointDef.bodyA = _player->getBody();
	_player->setFrictionJoint(std::shared_ptr<b2FrictionJoint>((b2FrictionJoint*)(_world->getWorld()->CreateJoint(&jointDef))));

	for (int i = 0; i < _enemies.size(); i++) {
		jointDef.bodyA = _enemies[i]->getBody();
		_enemies[i]->setFrictionJoint(std::shared_ptr<b2FrictionJoint>((b2FrictionJoint*)(_world->getWorld()->CreateJoint(&jointDef))));
	}

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