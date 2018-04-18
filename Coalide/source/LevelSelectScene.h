//
//	LevelSelectScene.h
//	Coalide
//
#ifndef __LEVEL_SELECT_SCENE_H__
#define __LEVEL_SELECT_SCENE_H__
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
class LevelSelectScene : public Scene {
protected:
	/** The asset manager for this game mode. */
	std::shared_ptr<AssetManager> _assets;

	// CONTROLLERS
	/** Controller for abstracting out input across multiple platforms */
	InputController _input;

	// VIEW
	/** Reference to the physics root of the scene graph */
	std::shared_ptr<Node> _rootnode;

	// BUTTONS
	std::shared_ptr<cugl::Button> _backButton;

	std::shared_ptr<cugl::Button> _lvl0;
	std::shared_ptr<cugl::Button> _lvl1;
	std::shared_ptr<cugl::Button> _lvl2;
	std::shared_ptr<cugl::Button> _lvl3;
	std::shared_ptr<cugl::Button> _lvl4;
	std::shared_ptr<cugl::Button> _lvl5;

	int _lvlSelected = 0;
	bool _didClickLevel;
	bool _didClickBack;
	
	/** Whether or not debug mode is active */
	bool _debug;

	/** WHether or not the level is reloading after a loss or reset */
	bool _reloading;


public:
#pragma mark -
#pragma mark Constructors
	/**
	* Creates a new game world with the default values.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	LevelSelectScene() : Scene() { };

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~LevelSelectScene() { dispose(); }

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	bool didClickBack() { return _didClickBack; }
	bool didClickLevel() { return _didClickLevel; }
	int getCurrentLevel() { return _lvlSelected; }

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
	bool init(const std::shared_ptr<AssetManager>& assets, InputController input);


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

};

#endif /* __RD_GAME_MODE_H__ */
