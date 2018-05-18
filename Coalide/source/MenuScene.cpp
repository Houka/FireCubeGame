#include "MenuScene.h"



/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void MenuScene::dispose() {
	if (_active) {
		CULog("disposing menu scene");
		_input.dispose();
		Scene::dispose();
	}
}

#pragma mark -
#pragma mark Constructors
/**
* Initializes the controller contents, and starts the game
*
* The constructor does not allocate any objects or memory.  This allows
* us to have a non-pointer reference to this controller, reducing our
* memory allocation.  Instead, allocation happens in this method.
*
* The game world is scaled so that the screen coordinates do not agree
* with the Box2d coordinates.  The bounds are in terms of the Box2d
* world, not the screen.
*
* @param assets    The (loaded) assets for this game mode
* @param rect      The game bounds in Box2d coordinates
* @param gravity   The gravitational force on this Box2d world
*
* @return  true if the controller is initialized properly, false otherwise.
*/
bool MenuScene::init(const std::shared_ptr<AssetManager>& assets, InputController input) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= GAME_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}

	else if (!Scene::init(dimen)) {
		return false;
	}

	// assets and input come from the CoalideApp level
	_assets = assets;
	_input = input;

	// Initialize the controllers used in the game mode
	_input.init();

	// Set up the scene graph
	createSceneGraph(dimen);

	_didClickStart = false;
	_didClickLevels = false;

	return true;
}

#pragma mark -
#pragma mark Scene Graph
/**
* Creates the scene graph.
*/
void MenuScene::createSceneGraph(Size dimen) {
	_rootnode = Node::alloc();
	_rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_rootnode->setPosition(Vec2::ZERO);

	// create the menu background texture
	cugl::Rect screen = cugl::Rect(0., 0., 500., 500.);
	std::shared_ptr<cugl::PolygonNode> _background = PolygonNode::allocWithTexture(_assets->get<Texture>("menu"));
	float xMax = getCamera()->getViewport().getMaxX();
	float yMax = getCamera()->getViewport().getMaxY();
	_background->setPosition(xMax / 2., yMax / 2.);
	float textureWidth = _background->getTexture()->getWidth();
	float textureHeight = _background->getTexture()->getHeight();
	_background->setScale(xMax/textureWidth, yMax/textureHeight);

	// create the buttons
	std::shared_ptr<cugl::Node> _startNode = PolygonNode::allocWithTexture(_assets->get<Texture>("start_button"));
	std::shared_ptr<cugl::Node> _levelsNode = PolygonNode::allocWithTexture(_assets->get<Texture>("levels_button"));

	//_startNode->setPosition(50., 50);
	_startButton = cugl::Button::alloc(_startNode);
	_startButton->setPosition(textureWidth / 4, textureHeight / 16.);

	_levelsButton = cugl::Button::alloc(_levelsNode);
	_levelsButton->setPosition(textureWidth / 1.8, textureHeight / 16.);

	_startButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		} else {
			_didClickStart = true;
		}
	});

	_levelsButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			_didClickLevels = true;
		}
	});

	_startButton->activate(2);
	_levelsButton->activate(3);

	_rootnode->addChild(_background, UNIT_PRIORITY);
	_background->addChild(_startButton, UNIT_PRIORITY);
	_background->addChild(_levelsButton, UNIT_PRIORITY);

	addChild(_rootnode, 0);
	
	_rootnode->setContentSize(dimen);

}

#pragma mark -
#pragma mark Gameplay Handling
/**
* Executes the core gameplay loop of this world.
*
* This method contains the specific update code for this mini-game. It does
* not handle collisions, as those are managed by the parent class WorldController.
* This method is called after input is read, but before collisions are resolved.
* The very last thing that it should do is apply forces to the appropriate objects.
*
* @param  dt    Number of seconds since last animation frame
*/
void MenuScene::update(float dt) {
	// Check to see if new level loaded yet
	_input.update(dt);

	_didClickStart = false;
	_didClickLevels = false;

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}
}

