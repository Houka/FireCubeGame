//
//	GameState.h
//	Coalide
//
#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__
#include <cugl/cugl.h>
#include <cugl/2d/physics/CUObstacleWorld.h>
#include <vector>
#include <cugl/assets/CUAsset.h>
#include <cugl/io/CUJsonReader.h>
#include "Constants.h"

using namespace cugl;

/** Forward references to the various classes used by this level */
class PlayerModel;
class EnemyModel;
class TileModel;
class ObjectModel;

#pragma mark -
#pragma mark Game State
/**
* Class that represents a dynamically loaded level in the game
*
* This class is a subclass of Asset so that we can use it with a GenericLoader. As with
* all assets, this class SHOULD NOT make any references to AssetManager in the load/unload
* methods. Assets should be treated as if they load in parallel, not in sequence.  Therefore,
* it is unsafe to assume that one asset loads before another.  If this level needs to connect
* to other assets (sound, images, etc.) this should take place after asset loading, such as
* during scene graph initialization.
*/
class GameState {
protected:
	Rect _bounds;
	Vec2 _gravity;
	Vec2 _scale;

	std::shared_ptr<Node> _rootnode;
	std::shared_ptr<Node> _worldnode;
	std::shared_ptr<Node> _debugnode;

	std::shared_ptr<ObstacleWorld> _world;
    std::shared_ptr<PlayerModel> _player;
	std::vector<std::shared_ptr<EnemyModel>> _enemies;
	std::vector<std::shared_ptr<ObjectModel>> _objects;
	std::vector<std::shared_ptr<TileModel>> _tiles;
	std::vector<std::shared_ptr<EnemyModel>> _spores;

	int** _board;
	std::shared_ptr<TileModel>** _tileBoard;

	// BUTTONS
	std::shared_ptr<cugl::Node> _uiNode;
	std::shared_ptr<cugl::Node> _gameOverScreen;
	std::shared_ptr<cugl::Node> _gameOverText;
	std::shared_ptr<cugl::Node> _winScreen;
	std::shared_ptr<cugl::Node> _winText;
	std::shared_ptr<cugl::Button> _pauseButton;
	std::shared_ptr<cugl::Button> _muteButton;
	std::shared_ptr<cugl::Button> _menuButton;
	std::shared_ptr<cugl::Button> _playButton;
	std::shared_ptr<cugl::Button> _quitButton;
	std::shared_ptr<cugl::Button> _restartButton;
	std::shared_ptr<cugl::Button> _nextButton;


	bool _didClickMenu = false;
	bool _didClickRestart = false;
	bool _didClickNext = false;
	bool _didClickMute = false;
	bool _isPaused = false;

	// UI constants:
	//cugl::Vec2 FAR_FAR_AWAY = cugl::Vec2(0,10000);
	//cugl::Vec2 LOSE_WIN_SCREEN_LOCATION = cugl::Vec2(-400,-500);
	//cugl::Vec2 QUIT_BUTTON_LOCATION = cugl::Vec2(-400, -200);
	//cugl::Vec2 REPLAY_BUTTON_LOCATION = cugl::Vec2(-200, -200);
	//cugl::Vec2 NEXT_BUTTON_LOCATION = cugl::Vec2(400, -200);
	//cugl::Vec2 LOSE_WIN_SCALE = cugl::Vec2(.4,.4);

	std::shared_ptr<cugl::AssetManager> _assets;

	/**
	* Clears the root scene graph node for this level
	*/
	void clearRootNode();


public:
#pragma mark -
#pragma mark Constructors
	/**
	* Creates a new game state representing an empty level.
	*/
	GameState() { }

	/**
	* Destroys this level, releasing all resources.
	*/
	virtual ~GameState() { dispose(); }

	/**
	* Destroys this level, releasing all resources.
	*/
	void dispose();

	bool didClickMenu() { return _didClickMenu; }
	bool didClickRestart() { return _didClickRestart; }
	bool didClickNext() { return _didClickNext; }
	bool didClickMute() { return _didClickMute; }
	bool isPaused() { return _isPaused; }
	void didPause();
	void resetDidClickMenu() { _didClickMenu = false; }
	void resetDidClickRestart() { _didClickRestart = false; }
	void resetDidClickNext() { _didClickNext = false; }
	void resetDidClickMute() { _didClickMute = false; }

	void setUIPosition(Vec2 pos) { _uiNode->setPosition(pos); }

	std::shared_ptr<Node> getUINode() { return _uiNode; }

	/**
	* Create a new game state. 
	*/
	bool init() { return true; }

#pragma mark -
#pragma mark Static Constructors
	/**
	* Creates a new empty level.
	*
	* @return  an empty level.
	*/
	static std::shared_ptr<GameState> alloc() {
		std::shared_ptr<GameState> result = std::make_shared<GameState>();
		return (result->init() ? result : nullptr);
	}


#pragma mark Model Access
	/**
	* Returns the player in this game level.
	*
	* @return the player in this game level.
	*/
	std::shared_ptr<PlayerModel>& getPlayer() { return _player; }

	/**
	* Sets the player in this game level.
	*/
	void setPlayer(std::shared_ptr<PlayerModel> player) { _player = player; }

	/**
	* Returns the active enemies in this game level.
	*
	* @return the active enemies in this game level.
	*/
	std::vector<std::shared_ptr<EnemyModel>>& getEnemies() { return _enemies; }

	/**
	* Sets the active enemies in this game level.
	*/
	void setEnemies(std::vector<std::shared_ptr<EnemyModel>> enemies) { _enemies = enemies; }

	/**
	* Returns the active enemies in this game level.
	*
	* @return the active enemies in this game level.
	*/
	std::vector<std::shared_ptr<EnemyModel>>& getSpores() { return _spores; }

	/**
	* Sets the active enemies in this game level.
	*/
	void setSpores(std::vector<std::shared_ptr<EnemyModel>> spores) { _spores = spores; }

	/**
	* Returns the active enemies in this game level.
	*
	* @return the active enemies in this game level.
	*/
	std::vector<std::shared_ptr<ObjectModel>>& getObjects() { return _objects; }

	/**
	* Sets the active enemies in this game level.
	*/
	void setObjects(std::vector<std::shared_ptr<ObjectModel>> objects) { _objects = objects; }

	/**
	* Returns the floor tiles in this game level.
	*
	* @return the floor tiles in this game level.
	*/
	std::vector<std::shared_ptr<TileModel>>& getTiles() { return _tiles; }

	/**
	* Sets the floor tiles in this game level.
	*/
	void setTiles(std::vector<std::shared_ptr<TileModel>> tiles) { _tiles = tiles; }

	/**
	* Returns the physics world of this game level.
	*
	* @return the physics world of this game level.
	*/
	std::shared_ptr<ObstacleWorld>& getWorld() { return _world; }

	/**
	* Sets the physics world of this game level.
	*/
	void setWorld(std::shared_ptr<ObstacleWorld> world) { _world = world; }

	/**
	* Returns the physics world of this game level.
	*
	* @return the physics world of this game level.
	*/
	int** getBoard() { return _board; }

	/**
	* Sets the physics world of this game level.
	*/
	void setBoard(int** board) { _board = board; }

	std::shared_ptr<TileModel>** getTileBoard() { return _tileBoard; }

	/**
	* Sets the physics world of this game level.
	*/
	void setTileBoard(std::shared_ptr<TileModel>** tileBoard) { _tileBoard = tileBoard; }


#pragma mark Physics Attributes
	/**
	* Returns the bounds of this level in physics coordinates
	*
	* @return the bounds of this level in physics coordinates
	*/
	const Rect& getBounds() const { return _world->getBounds(); }

	/**
	* Returns the global gravity for this level
	*
	* @return the global gravity for this level
	*/
	const Vec2& getGravity() const { return _gravity; }

	/**
	* Sets the global gravity for this level
	*/
	void setGravity(Vec2 gravity) { _gravity = gravity; }


#pragma mark Drawing Methods
	/**
	* Returns the scene graph node for drawing purposes.
	*
	* @return the scene graph node for drawing purposes.
	*/
	const std::shared_ptr<Node>& getRootNode() const { return _rootnode; }
    
    const std::shared_ptr<Node>& getWorldNode() const { return _worldnode; }
    
//    void setPlayerTexture() { _player->getNode()->setTexture(); }

	/**
	* Sets the scene graph node for drawing purposes.
	*
	* @param value  the scene graph node for drawing purposes.
	*
	* @retain  a reference to this scene graph node
	* /Users/kylerruvane/Documents/4152/FireCubeGame/Coalide/source/GameScene.cpp@release the previous scene graph node used by this object
	*/
    void setRootNode(const std::shared_ptr<Node>& node);

	void addSporeNode(const std::shared_ptr<EnemyModel> spore);

	void showGameOverScreen(bool show);
	void showWinScreen(bool show);

	/**
	* Returns the drawing scale for this game level
	*
	* @return the drawing scale for this game level
	*/
	const Vec2 getDrawScale() const { return _scale; }

	/**
	* Sets the drawing scale for this game level
	*
	* @param value  the drawing scale for this game level
	*/
	void setDrawScale(Vec2 scale) { _scale = scale; }

	/**
	* Sets the loaded assets for this game level
	*
	* @param assets the loaded assets for this game level
	*/
	void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets; }

	/**
	* Toggles whether to show the debug layer of this game world.
	*
	* The debug layer displays wireframe outlines of the physics fixtures.
	*
	* @param  flag whether to show the debug layer of this game world
	*/
	void showDebug(bool flag);
};

#endif /* defined(__GAME_STATE_H__) */
