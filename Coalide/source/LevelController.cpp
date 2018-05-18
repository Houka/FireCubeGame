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

#define TILE_BORDER  .0007

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
	auto levelInfo = json->get("levelInfo");
	int canvasH = 64 * levelInfo->get("rows")->asInt();
	int canvasW = 64 * levelInfo->get("cols")->asInt();
    
    
	auto tilesets = json->get(TILESETS_FIELD);

    int tileW = 64;
    int tileH = 64;
	_tileDim.set(tileW, tileH);

    int worldTilesetW = tileW * 21;
    int worldTilesetH = tileH * 8;
	_worldTilesetDim.set(worldTilesetW, worldTilesetH);

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

//    // Create the player and enemy(s)
    if (!loadUnits(json)) {
        CUAssertLog(false, "Failed to load player");
        return false;
    }

	addFrictionJoints();

	buildGameState();

	return true;
}

bool LevelController::loadTerrain(const std::shared_ptr<JsonValue>& json) {
	bool success = true;

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
    
    auto levelInfo = json->get("levelInfo");
    int rows = levelInfo->get("rows")->asInt();
    int cols = levelInfo->get("cols")->asInt();
    
    auto terrain = json->get("terrain");
    auto terrrainTypesArray = terrain->get("types");
    
    auto textures = json->get("textures");
    auto sandTextureArray = textures->get("sand");
    auto iceTextureArray = textures->get("ice");
    auto dirtTextureArray = textures->get("grass");
    auto waterBaseArray = textures->get("water");
    auto waterDecalArray = textures->get("waterDecals");
    
    
    _board = new int*[worldH];
    _tileBoard = new std::shared_ptr<TileModel>*[worldH];

    for (int i = 0; i < worldH; i++) {
        _board[i] = new int[worldW];
        _tileBoard[i] = new std::shared_ptr<TileModel>[worldW];
    }

    for(int r = 0; r<rows; r++){
        auto typesArray = terrrainTypesArray->get(r);
        auto innerSand = sandTextureArray->get(r);
        auto innerIce = iceTextureArray->get(r);
        auto innerDirt = dirtTextureArray->get(r);
        auto innerWaterBase = waterBaseArray->get(r);
        auto innerWaterDecal = waterDecalArray->get(r);

        for(int c = 0; c<cols; c++){
            
            std::string type = typesArray->get(c)->asString();
            if(type == "empty"){
                _board[rows - 1 - r][c] = 0;
                int waterDecal = innerWaterDecal->get(c)->asInt();
                if(waterDecal != -1){
                    std::shared_ptr<TileModel> tile = TileModel::alloc(Vec2(c + .5, (rows - r) - .5), UNIT_DIM);
                    tile->setType(TILE_TYPE::WATER);
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (waterDecal / 21));
                    double subTextureX = (waterDecal % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setWaterDecalSubTexture((subTextureX / 7) + TILE_BORDER, (endX / 7), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                    tile->setWaterTextureKey("tileset_water.png");
                    _tiles.push_back(tile);
                    //_tileBoard[r][c] = tile;
                }
            }
            else {
                std::shared_ptr<TileModel> tile = TileModel::alloc(Vec2(c + .5, (rows - r) - .5), UNIT_DIM);
                int sandTexture = innerSand->get(c)->asInt();
                int iceTexture = innerIce->get(c)->asInt();
                int dirtTexture = innerDirt->get(c)->asInt();
                int waterBase = innerWaterBase->get(c)->asInt();
                int waterDecal = innerWaterDecal->get(c)->asInt();
                tile->setWaterTextureKey("tileset_water.png");
                if(type == "sand"){
                    _board[rows - 1 - r][c] = 20;
                    tile->setType(TILE_TYPE::SAND);
                    tile->setSandTextureKey("tileset_forest.png");
                    tile->setIceTextureKey("tileset_forest.png");
                    tile->setDirtTextureKey("tileset_forest.png");
                
                }
                if(type == "ice"){
					_board[rows - 1 - r][c] = 2;
                    tile->setType(TILE_TYPE::ICE);
                    tile->setIceTextureKey("tileset_forest.png");
                    tile->setDirtTextureKey("tileset_forest.png");
                }
                if(type == "dirt"){
					_board[rows - 1 - r][c] = 10;
                    tile->setType(TILE_TYPE::GRASS);
                    tile->setDirtTextureKey("tileset_forest.png");
                }
                if(sandTexture != -1){
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (sandTexture / 21));
                    double subTextureX = (sandTexture % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setSandSubTexture((subTextureX / 21) + TILE_BORDER, (endX / 21), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                }
                if(iceTexture != -1){
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (iceTexture / 21));
                    double subTextureX = (iceTexture % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setIceSubTexture((subTextureX / 21) + TILE_BORDER, (endX / 21), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                }
                if(dirtTexture != -1){
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (dirtTexture / 21));
                    double subTextureX = (dirtTexture % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setDirtSubTexture((subTextureX / 21) + TILE_BORDER, (endX / 21), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                }
                if(waterBase != -1){
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (waterBase / 21));
                    double subTextureX = (waterBase % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setWaterBaseSubTexture((subTextureX / 7) + TILE_BORDER, (endX / 7), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                }
                if(waterDecal != -1){
                    //gives box to specify texture into texture atlas
                    double subTextureY = ((int) (waterDecal / 21));
                    double subTextureX = (waterDecal % 21);
                    double endY = subTextureY + 1;
                    double endX = subTextureX + 1;
                    tile->setWaterDecalSubTexture((subTextureX / 7) + TILE_BORDER, (endX / 7), (subTextureY / 8) + TILE_BORDER, (endY / 8));
                }
                _tiles.push_back(tile);
                _tileBoard[rows - 1 - r][c] = tile;
            }
        }
    }
	return success;
}

bool LevelController::loadUnits(const std::shared_ptr<cugl::JsonValue>& json) {
	bool success = true;
	
    auto levelInfo = json->get("levelInfo");
    int rows = levelInfo->get("rows")->asInt();
    
    auto objects = json->get("objects");
    
    // player
    auto player = objects->get("player");
    _player = PlayerModel::alloc(Vec2(player->get("col")->asInt() + .5, rows - player->get("row")->asInt() - .5), PLAYER_DIM);
    _world->addObstacle(_player);
    
    //acorns
    auto acornArray = objects->get("acorns");
    for(auto acorn : acornArray->_children){
        std::shared_ptr<EnemyModel> enemy;

        int r = acorn->get("row")->asInt();
        int c = acorn->get("col")->asInt();
        enemy = EnemyModel::alloc(Vec2(c + .5, (rows - r) - .5), ACORN_DIM);
        enemy->setTextureKey(ACORN);
		enemy->setAcorn();

        _world->addObstacle(enemy);
        _enemies.push_back(enemy);
    }
    
    //onions
    auto onionArray = objects->get("onions");
    for(auto onion : onionArray->_children){
        std::shared_ptr<EnemyModel> enemy;
        
        int r = onion->get("row")->asInt();
        int c = onion->get("col")->asInt();
        enemy = EnemyModel::alloc(Vec2(c + .5, (rows - r) - .5), ONION_DIM);
        enemy->setTextureKey(ONION);
		enemy->setDensity(2.5);
		enemy->setOnion();

        _world->addObstacle(enemy);
        _enemies.push_back(enemy);
    }
    
    //mushrooms
    auto mushroomArray = objects->get("mushrooms");
    for(auto mushroom : mushroomArray->_children){
        std::shared_ptr<EnemyModel> enemy;
        
        int r = mushroom->get("row")->asInt();
        int c = mushroom->get("col")->asInt();
        enemy = EnemyModel::alloc(Vec2(c + .5, (rows - r) - .5), MUSHROOM_DIM);
        enemy->setTextureKey(MUSHROOM);
		enemy->setDensity(2.5);
        enemy->setMushroom();
        
        b2Filter filter;
        filter.categoryBits = CATEGORY_MUSHROOM;
        filter.maskBits = ~CATEGORY_SPORE;
        filter.groupIndex = NULL;
        enemy->setFilterData(filter);
        
        _world->addObstacle(enemy);
        _enemies.push_back(enemy);
    }
    
	//breakable crates
	/*auto breakableCrateArray = objects->get("breakableCrates");
	for (auto breakableCrate : breakableCrateArray->_children) {
		std::shared_ptr<ObjectModel> object;

		int r = breakableCrate->get("row")->asInt();
		int c = breakableCrate->get("col")->asInt();
		object = ObjectModel::alloc(Vec2(c + .5, (rows - r) - .5), UNIT_DIM);
		object->setTextureKey(BREAKABLE_NAME);
		object->setName(BREAKABLE_NAME);
		object->setBodyType(b2_dynamicBody);
		object->setType(OBJECT_TYPE::BREAKABLE);

		_world->addObstacle(object);
		_objects.push_back(object);
	}*/

    //movable crates
    auto crateArray = objects->get("crates");
    for(auto crate : crateArray->_children){
        std::shared_ptr<ObjectModel> object;
        
        int r = crate->get("row")->asInt();
        int c = crate->get("col")->asInt();
        object = ObjectModel::alloc(Vec2(c + .5, (rows - r) - .5), UNIT_DIM);
        object->setTextureKey(BREAKABLE_NAME);
        object->setName(BREAKABLE_NAME);
        object->setBodyType(b2_dynamicBody);
		object->setType(OBJECT_TYPE::BREAKABLE);
        
        _world->addObstacle(object);
        _objects.push_back(object);
    }
    
    //rocks
    auto rockArray = objects->get("rocks");
    for(auto rock : rockArray->_children){
        std::shared_ptr<ObjectModel> object;
        
        int r = rock->get("row")->asInt();
        int c = rock->get("col")->asInt();
        object = ObjectModel::alloc(Vec2(c + .5, (rows - r) - .5), UNIT_DIM);
        object->setTextureKey(IMMOBILE_NAME);
        object->setName(IMMOBILE_NAME);
        object->setBodyType(b2_staticBody);
		object->setType(OBJECT_TYPE::IMMOBILE);

        _world->addObstacle(object);
        _objects.push_back(object);
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
