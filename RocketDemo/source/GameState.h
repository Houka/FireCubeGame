//
//  RDRocketModel.h
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
#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__
#include <cugl/cugl.h>
#include <cugl/assets/CUAsset.h>
#include <cugl/io/CUJsonReader.h>

class TileModel;
class PlayerModel;

class GameState : public cugl::Asset {
public:
	/**
	* Enumeration to identify the rocket afterburner
	*/
	enum class TILE_TYPE : int {
		WATER,
		ISLAND,
		ISLAND_BASE,
		LAND,
		NW_LAND,
		N_LAND,
		NE_LAND,
		E_LAND,
		SE_LAND,
		S_LAND,
		SW_LAND,
		W_LAND,
		L_LAND_BASE,
		LAND_BASE,
		R_LAND_BASE
	};

protected:
	std::shared_ptr<GameState> _gameState;
	cugl::Size _canvasDim;
	cugl::Size _tileDim;
	cugl::Rect _worldDim;
	TILE_TYPE** _world;
	std::shared_ptr<cugl::AssetManager> _assets;
	std::shared_ptr<cugl::JsonValue> _tilesetsInfo;
	std::shared_ptr<cugl::ObstacleWorld> _physicsWorld;
	std::vector<std::shared_ptr<TileModel>> _tiles;
	/** The root node of this level */
	std::shared_ptr<cugl::Node> _root;
	/** Reference to the physics root of the scene graph */
	std::shared_ptr<cugl::Node> _worldnode;
	/** Reference to the debug root of the scene graph */
	std::shared_ptr<cugl::Node> _debugnode;
	/** The level drawing scale (difference between physics and drawing coordinates) */
	cugl::Vec2 _scale;
	std::shared_ptr<PlayerModel> _player;

public:
#pragma mark Constructors
	/**
	* Creates a game state.
	*
	* NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	* the heap, use one of the static constructors instead.
	*/
	GameState(void);

	/**
	* Destroys this game state, releasing all resources.
	*/
	virtual ~GameState(void);

	/**
	* Disposes all resources and assets of this game state.
	*
	* Any assets owned by this object will be immediately released.  Once
	* disposed, a game state may not be used until it is initialized again.
	*/
	void dispose();


#pragma mark Static Constructors
	/**
	* Creates a new game level with no source file.
	*
	* The source file can be set at any time via the setFile() method. This method
	* does NOT load the asset.  You must call the load() method to do that.
	*
	* @return  an autoreleased level file
	*/
	static std::shared_ptr<GameState> alloc() {
		std::shared_ptr<GameState> result = std::make_shared<GameState>();
		return (result->init("") ? result : nullptr);
	}

	/**
	* Returns a newly allocated game state.
	*
	* @return a newly allocated game state.
	*/
	static std::shared_ptr<GameState> alloc(std::string file) {
		std::shared_ptr<GameState> result = std::make_shared<GameState>();
		return (result->init(file) ? result : nullptr);
	}


#pragma mark -
#pragma mark Accessors
	std::shared_ptr<GameState> getGameState() const { return _gameState; }


#pragma mark -
#pragma mark Asset Loading
	/**
	* Loads this game level from the source file
	*
	* This load method should NEVER access the AssetManager.  Assets are loaded in
	* parallel, not in sequence.  If an asset (like a game level) has references to
	* other assets, then these should be connected later, during scene initialization.
	*
	* @param file the name of the source file to load from
	*
	* @return true if successfully loaded the asset from a file
	*/
	virtual bool preload(const std::string& file) override;


	/**
	* Loads this game level from a JsonValue containing all data from a source Json file.
	*
	* This load method should NEVER access the AssetManager.  Assets are loaded in
	* parallel, not in sequence.  If an asset (like a game level) has references to
	* other assets, then these should be connected later, during scene initialization.
	*
	* @param json the json loaded from the source file to use when loading this game level
	*
	* @return true if successfully loaded the asset from the input JsonValue
	*/
	virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;

	/**
	* Unloads this game level, releasing all sources
	*
	* This load method should NEVER access the AssetManager.  Assets are loaded and
	* unloaded in parallel, not in sequence.  If an asset (like a game level) has
	* references to other assets, then these should be disconnected earlier.
	*/
	void unload();

	/**
	* Clears the root scene graph node for this level
	*/
	void clearRootNode();

	/**
	* Loads the singular rocket object
	*
	* The rocket will will be stored in _rocket field and retained.
	* If the rocket fails to load, then _rocket will be nullptr.
	*
	* @param  reader   a JSON reader with cursor ready to read the rocket
	*
	* @retain the rocket
	* @return true if the rocket was successfully loaded
	*/
	bool loadWorld(const std::shared_ptr<cugl::JsonValue>& json, int worldW, int worldH);
	
	bool loadTile(cugl::Vec2 tilePos, cugl::Size tileDim, TILE_TYPE tileType);

	bool loadEntities(const std::shared_ptr<cugl::JsonValue>& json);

	cugl::Rect getWorldDim() { return _worldDim; }

	std::shared_ptr<cugl::ObstacleWorld> getPhysicsWorld() { return _physicsWorld; }

	std::shared_ptr<PlayerModel> getPlayer() { return _player; }
	
	/**
	* Sets the scene graph node for drawing purposes.
	*
	* The scene graph is completely decoupled from the physics system.  The node
	* does not have to be the same size as the physics body. We only guarantee
	* that the node is positioned correctly according to the drawing scale.
	*
	* @param value  the scene graph node for drawing purposes.
	*
	* @retain  a reference to this scene graph node
	* @release the previous scene graph node used by this object
	*/
	void setRootNode(const std::shared_ptr<cugl::Node>& root);

	/**
	* Sets the loaded assets for this game level
	*
	* @param assets the loaded assets for this game level
	*/
	void setAssets(const std::shared_ptr<cugl::AssetManager>& assets) { _assets = assets; }

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
	* have different z-orders whenever possible.  This will cut down on the
	* amount of drawing done
	*
	* param obj    The physics object to add
	* param node   The scene graph node to attach it to
	* param zOrder The drawing order
	*/
	void addObstacle(const std::shared_ptr<cugl::Obstacle>& obj, const std::shared_ptr<cugl::Node>& node, int zOrder);
};
#endif /* __RD_ROCKET_MODEL_H__ */

