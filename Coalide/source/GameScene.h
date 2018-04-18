//
//	GameScene.h
//	Coalide
//
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include <cugl/cugl.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <vector>
#include "InputController.h"
#include "CollisionController.h"
#include "AIController.h"
#include "GameState.h"
#include "LevelController.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "ObjectModel.h"

using namespace cugl;

/** Forward references */
class GameState;


/**
* This class is the primary gameplay constroller for the demo.
*
* A world has its own objects, assets, and input controller.  Thus this is
* really a mini-GameEngine in its own right.  As in 3152, we separate it out
* so that we can have a separate mode for the loading screen.
*/
class GameScene : public Scene {
protected:
	/** The asset manager for this game mode. */
	std::shared_ptr<AssetManager> _assets;

	// CONTROLLERS
	/** Controller for abstracting out input across multiple platforms */
	InputController _input;
	/** Controller for handling collisions using Box2D */
	CollisionController _collisions;
	/** Controller for determining AI behavior */
	AIController _ai;

	// VIEW
	/** Reference to the physics root of the scene graph */
	std::shared_ptr<Node> _rootnode;
	/** Reference to the win message label */
	std::shared_ptr<Label> _winnode;
	/** Reference to the lose message label */
	std::shared_ptr<Label> _losenode;
	/** Reference to the reset message label */
	std::shared_ptr<Label> _loadnode;
	
	/** The current game state */
	std::shared_ptr<GameState> _gamestate;

	/** Whether we have completed this "game" */
	bool _complete;
	/** Whether or not debug mode is active */
	bool _debug;
	/** Whether or not we have lost */
	bool _gameover;
	/** WHether or not the level is reloading after a loss or reset */
	bool _reloading;

	/** Number of enemies remaining. */
	int _enemyCount;

	/** Level key for loading correct tileset. */
	std::string _levelKey;

	/**
	* Activates world collision callbacks on the given physics world and sets the onBeginContact and beforeSolve callbacks
	*
	* @param world the physics world to activate world collision callbacks on
	*/
	void activateWorldCollisions();


public:
#pragma mark -
#pragma mark Constructors
	/**
	* Creates a new game world with the default values.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	GameScene() : Scene(), _complete(false), _debug(false), _gameover(false), _reloading(false) { };

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~GameScene() { dispose(); }

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	/**
	* Initializes the controller contents, and starts the game
	*
	* The constructor does not allocate any objects or memory.  This allows
	* us to have a non-pointer reference to this controller, reducing our
	* memory allocation.  Instead, allocation happens in this method.
	*
	* The game world is scaled so that the screen coordinates do not agree
	* with the Box2d coordinates.  This initializer uses the default scale.
	*
	* @param assets    The (loaded) assets for this game mode
	*
	* @return true if the controller is initialized properly, false otherwise.
	*/
	bool init(const std::shared_ptr<AssetManager>& assets, InputController input, std::string levelKey);


#pragma mark -
#pragma mark State Access
	/**
	* Returns true if the gameplay controller is currently active
	*
	* @return true if the gameplay controller is currently active
	*/
	bool isActive() const { return _active; }

	/**
	* Returns true if debug mode is active.
	*
	* If true, all objects will display their physics bodies.
	*
	* @return true if debug mode is active.
	*/
	bool isDebug( ) const { return _debug; }

	/**
	* Sets whether debug mode is active.
	*
	* If true, all objects will display their physics bodies.
	*
	* @param value whether debug mode is active.
	*/
	void setDebug(bool value) { _debug = value; _gamestate->showDebug(value); }

	/**
	* Returns true if the level is completed.
	*
	* If true, the level will advance after a countdown
	*
	* @return true if the level is completed.
	*/
	bool isComplete() const { return _complete; }

	/**
	* Returns true if the level is lost.
	*
	* If true, the game over screen will appear.
	*
	* @return true if the level is lost.
	*/
	bool isGameOver() const { return _gameover; }

	/**
	* Sets whether the level is completed.
	*
	* If true, the level will advance after a countdown
	*
	* @param value whether the level is completed.
	*/
	void setComplete(bool value) { _complete = value; _winnode->setVisible(value); }

	/**
	* Sets whether the level is lost.
	*
	* If true, the game over screen will appear
	*
	* @param value whether the level is completed.
	*/
	void setGameOver(bool value) { _gameover = value; _losenode->setVisible(value); }


#pragma mark -
#pragma mark Scene Graph
	void createSceneGraph(Size dimen);

#pragma mark -
#pragma mark Gameplay Handling
	/**
	* The method called to update the game mode.
	*
	* This method contains any gameplay code that is not an OpenGL call.
	*
	* @param timestep  The amount of time (in seconds) since the last frame
	*/
	void update(float dt);

	void updateFriction();

	void removeEnemy(std::shared_ptr<EnemyModel> enemy);
	
	void removeObject(std::shared_ptr<ObjectModel> object);

	/**
	* Resets the status of the game so that we can play again.
	*/
	void reset(const std::string& file);
};

#endif /* __RD_GAME_MODE_H__ */
