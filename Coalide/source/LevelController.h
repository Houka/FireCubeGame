//
//	Coalide.h
//	Coalide
//
#ifndef __LEVEL_CONTROLLER_H__
#define __LEVEL_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameState.h"
#include "Constants.h"

using namespace cugl;

/** Forward references to classes */
class GameState;
class PlayerModel;
class EnemyModel;
class TileModel;
class ObjectModel;

class LevelController : public Asset {
protected:
	std::shared_ptr<AssetManager> _assets;
	std::shared_ptr<GameState> _gamestate;

	Rect _bounds;
	Size _tileDim;
	Size _worldTilesetDim;
	Size _waterTilesetDim;
	Vec2 _scale;

	std::shared_ptr<ObstacleWorld> _world;
	
	std::shared_ptr<PlayerModel> _player;
	std::vector<std::shared_ptr<EnemyModel>> _enemies;
	std::vector<std::shared_ptr<ObjectModel>> _objects;
	std::vector<std::shared_ptr<TileModel>> _tiles;
	std::shared_ptr<BoxObstacle> _terrain;

	int** _board;
	std::shared_ptr<TileModel>** _tileBoard;

	bool _levelBuilt;

public:
#pragma mark Constructors
	/**
	* Creates a game state.
	*
	* NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	* the heap, use one of the static constructors instead.
	*/
	LevelController() : Asset(), _world(nullptr), _player(nullptr), _gamestate(nullptr), _levelBuilt(false) { }

	/**
	* Destroys this game state, releasing all resources.
	*/
	virtual ~LevelController() { unload(); }

	/**
	* Disposes all resources and assets of this game state.
	*
	* Any assets owned by this object will be immediately released.  Once
	* disposed, a game state may not be used until it is initialized again.
	*/
	void dispose();


#pragma mark -
#pragma mark Asset Loading
	/**
	* Loads this game level from the source file
	*
	* @param file the name of the source file to load from
	*
	* @return true if successfully loaded the asset from a file
	*/
	virtual bool preload(const std::string& file) override;

	/**
	* Loads this game level from a JsonValue containing all data from a source Json file.
	*
	* @param json the json loaded from the source file to use when loading this game level
	*
	* @return true if successfully loaded the asset from the input JsonValue
	*/
	virtual bool preload(const std::shared_ptr<JsonValue>& json) override;

	/**
	* Unloads this game level, releasing all sources
	*/
	void unload();


#pragma mark -
#pragma mark Level Loading
	/** Loads the floor tiles */
	bool loadTerrain(const std::shared_ptr<JsonValue>& json);

	/** Loads a single floor tile */
	bool loadLandTile(Vec2 tilePos, float tileVal, TILE_TYPE tileType, std::shared_ptr<JsonValue>& layer);
	bool loadWaterTile(Vec2 tilePos, float tileVal, TILE_TYPE tileType, std::shared_ptr<JsonValue>& layer);

	/** Loads the player, enemies, and inanimate objects */
	bool loadUnits(const std::shared_ptr<JsonValue>& json);

	/** Adds friction joints between units and the ground to simulate top-down friction. */
	void addFrictionJoints();

	/** Builds a game state from the loaded level */
	void buildGameState();


#pragma mark -
#pragma mark Accessors
	/**
	* Returns the game state built from the loaded level.
	*
	* @return a pointer to the game state. */
	std::shared_ptr<GameState> getGameState() const { return _gamestate; }

	/**
	* Returns true if the leve's game state is done building.
	*/
	bool isLevelBuilt() { return _levelBuilt; }
};
#endif /* __LEVEL_CONTROLLER_H__ */

