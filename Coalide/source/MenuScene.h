#pragma once
#include "C:\Users\Paul\Documents\Cornell\Courses\Spring2018\CS4152\FireCubeGame\Coalide\cugl\include\cugl\2d\CUScene.h"
#include <cugl/cugl.h>
#include "InputController.h"
#include "Constants.h"
using namespace cugl;

class MenuScene : public Scene {
public:
	/** The asset manager for this game mode. */
	std::shared_ptr<AssetManager> _assets;

	// CONTROLLERS
	/** Controller for abstracting out input across multiple platforms */
	InputController _input;

	// VIEW
	/** Reference to the physics root of the scene graph */
	std::shared_ptr<Node> _rootnode;

#pragma mark -
#pragma mark Constructors

	/**
	* Creates a new menu screen with a background and buttons.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	MenuScene() : Scene() {};

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources.
	*/
	~MenuScene() { dispose(); }

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	/**
	* Initializes the controller contents, and shows the menu
	*
	* @param assets    The (loaded) assets for this game mode
	*
	* @return true if the controller is initialized properly, false otherwise.
	*/
	bool init(const std::shared_ptr<AssetManager>& assets, InputController input);

#pragma mark -
#pragma mark Scene Graph
	void createSceneGraph(Size dimen);
};

