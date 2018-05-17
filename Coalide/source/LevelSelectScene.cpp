//
//  LevelSelectScene.cpp
//  Coalide
//
#include "LevelSelectScene.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include "Constants.h"

#include <string>

using namespace cugl;

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
bool LevelSelectScene::init(const std::shared_ptr<AssetManager>& assets, InputController input) {
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


	_currentOffset = 0;
	_prevOffset = 0;

	// Initialize the controllers used in the game mode
	_input.init();

	// Set up the scene graph
	createSceneGraph(dimen);

	_didClickBack = false;
	_didClickLevel = false;

	// initialize the camera
	//cugl::Vec2 cameraPos = getCamera()->getPosition();
	//getCamera()->translate(gameCenter - cameraPos);
	return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void LevelSelectScene::dispose() {
	if (_active) {
		_input.dispose();
		_rootnode = nullptr;
		Scene::dispose();
	}
}


#pragma mark -
#pragma mark Scene Graph
/**
* Creates the scene graph.
*/
void LevelSelectScene::createSceneGraph(Size dimen) {
	_rootnode = Node::alloc();
	_rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_rootnode->setPosition(Vec2::ZERO);

	// create the menu background texture
	cugl::Rect screen = cugl::Rect(0., 0., 500., 500.);
	_background = PolygonNode::allocWithTexture(_assets->get<Texture>("map"));
	float xMax = getCamera()->getViewport().getMaxX();
	float yMax = getCamera()->getViewport().getMaxY();
	float textureWidth = _background->getTexture()->getWidth();
	float textureHeight = _background->getTexture()->getHeight();
	_scaledWidth = textureWidth*(yMax / textureHeight);
	_background->setPosition(_scaledWidth / 2., yMax / 2.);
	_background->setScale(yMax / textureHeight, yMax / textureHeight);
	_maxX = _scaledWidth / 2.;
	_minX = xMax - _scaledWidth / 2.;
	_prevOffset = 0;

	// create the buttons
	std::shared_ptr<cugl::Node> _backNode = PolygonNode::allocWithTexture(_assets->get<Texture>("menu_button"));
	std::shared_ptr<cugl::Node> _lvl0Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl1Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl2Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl3Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl4Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl5Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));

	//_startNode->setPosition(50., 50);
	_backButton = cugl::Button::alloc(_backNode);
	_lvl0 = cugl::Button::alloc(_lvl0Node);
	_lvl0->setScale(2, 2);
	_lvl1 = cugl::Button::alloc(_lvl1Node);
	_lvl1->setScale(2, 2);
	_lvl2 = cugl::Button::alloc(_lvl2Node);
	_lvl2->setScale(2, 2);
	_lvl3 = cugl::Button::alloc(_lvl3Node);
	_lvl3->setScale(2, 2);
	_lvl4 = cugl::Button::alloc(_lvl4Node);
	_lvl4->setScale(2, 2);
	_lvl5 = cugl::Button::alloc(_lvl5Node);
	_lvl5->setScale(2, 2);
	_backButton->setPosition(10, textureHeight*0.9);
	_lvl0->setPosition(textureWidth*.08, textureHeight*.26);
	_lvl1->setPosition(textureWidth*.08, textureHeight*.75);
	_lvl2->setPosition(textureWidth*.20, textureHeight*.55);
	_lvl3->setPosition(textureWidth*.17, textureHeight*.20);
	_lvl4->setPosition(textureWidth*.28, textureHeight*.26);
	_lvl5->setPosition(textureWidth*.32, textureHeight*.67);

	_backButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_didClickBack = true;
		}
	});

	_lvl0->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 0;
			_didClickLevel = true;
		}
	});
	_lvl1->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 1;
			_didClickLevel = true;
		}
	});
	_lvl2->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 2;
			_didClickLevel = true;
		}
	});
	_lvl3->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 3;
			_didClickLevel = true;
		}
	});
	_lvl4->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 4;
			_didClickLevel = true;
		}
	});
	_lvl5->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 5;
			_didClickLevel = true;
		}
	});


	_backButton->activate(4);
	_lvl0->activate(100);
	_lvl1->activate(101);
	_lvl2->activate(102);
	_lvl3->activate(103);
	_lvl4->activate(104);
	_lvl5->activate(105);

	_rootnode->addChild(_background, UNIT_PRIORITY);
	_background->addChild(_backButton, UNIT_PRIORITY);
	_background->addChild(_lvl0, UNIT_PRIORITY);
	_background->addChild(_lvl1, UNIT_PRIORITY);
	_background->addChild(_lvl2, UNIT_PRIORITY);
	_background->addChild(_lvl3, UNIT_PRIORITY);
	_background->addChild(_lvl4, UNIT_PRIORITY);
	_background->addChild(_lvl5, UNIT_PRIORITY);

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
void LevelSelectScene::update(float dt) {
	// Check to see if new level loaded yet
	_input.update(dt);

	_didClickBack = false;
	_didClickLevel = false;

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

	// handle scrolling on the level select screen
	float xPos = _background->getPositionX();
	if (_input.didStartSling()) {
		_prevOffset = _currentOffset;
		_currentOffset = _input.getCurrentAim().x;
	}
	else {
		if (_currentOffset != 0) { // if a finger is lifted mid-scroll, keep scrolling
			_prevOffset -= _currentOffset;
			_currentOffset = 0;
		}
		// smoothly slow down the scroll
		_prevOffset /= 1.2;
	}

	float newOffset = _currentOffset - _prevOffset;
	if (xPos + newOffset >= _maxX) {
		newOffset = _maxX - xPos;
	}
	if (xPos + newOffset <= _minX) {
		newOffset = _minX - xPos;
	}

	_background->setPositionX(xPos + newOffset);
}

