#pragma once
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

	// BUTTONS
	std::shared_ptr<cugl::Button> _startButton;
	std::shared_ptr<cugl::Button> _levelsButton;

	bool _didClickStart;
	bool _didClickLevels;

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

	bool didClickStart() { return _didClickStart; }
	bool didClickLevels() { return _didClickLevels; }

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



#pragma mark -
#pragma mark Progress Monitoring
	/**
	* The method called to update the game mode.
	*
	* This method updates the progress bar amount.
	*
	* @param dt  The amount of time (in seconds) since the last frame
	*/
	void update(float dt);

};

