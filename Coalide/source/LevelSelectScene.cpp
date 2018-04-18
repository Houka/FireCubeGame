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


	// Initialize the controllers used in the game mode
	_input.init();

	// Set up the scene graph
	createSceneGraph(dimen);

	_didClickBack = false;

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
	std::shared_ptr<cugl::PolygonNode> _background = PolygonNode::allocWithTexture(_assets->get<Texture>("map"));
	float xMax = getCamera()->getViewport().getMaxX();
	float yMax = getCamera()->getViewport().getMaxY();
	float textureWidth = _background->getTexture()->getWidth();
	float textureHeight = _background->getTexture()->getHeight();
	float scaledWidth = textureWidth*(yMax / textureHeight);
	_background->setPosition(scaledWidth / 2., yMax / 2.);
	_background->setScale(yMax / textureHeight, yMax / textureHeight);

	// create the buttons
	std::shared_ptr<cugl::Node> _backNode = PolygonNode::allocWithTexture(_assets->get<Texture>("menu_button"));

	//_startNode->setPosition(50., 50);
	_backButton = cugl::Button::alloc(_backNode);
	_backButton->setPosition(10, textureHeight*0.9);

	_backButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_didClickBack = true;
		}
	});


	_backButton->activate(4);

	_rootnode->addChild(_background, UNIT_PRIORITY);
	_background->addChild(_backButton, UNIT_PRIORITY);

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

	if (_input.didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}


	// update the camera
	//player->getNode()->getScene()->setOffset(cugl::Vec2(0,0));
	//cugl::Vec2 cameraPos = player->getNode()->getScene()->getCamera()->getPosition();
	//cugl::Vec2 playerPos = player->getNode()->getPosition();
	float cameraTransX;
	float cameraTransY;
	
	//cugl::Vec2 gameBound = cugl::Vec2(_gamestate->getBounds().size.getIWidth(), _gamestate->getBounds().size.getIHeight());
	//cugl::Vec2 gameBound = _gamestate->getBounds().size * 64;
	//float xMax = player->getNode()->getScene()->getCamera()->getViewport().getMaxX();
	//float yMax = player->getNode()->getScene()->getCamera()->getViewport().getMaxY();
	
	cugl::Vec2 boundBottom = Scene::screenToWorldCoords(cugl::Vec2());
	//cugl::Vec2 boundTop = Scene::screenToWorldCoords(cugl::Vec2(xMax,yMax));

	//cameraTransX = playerPos.x - cameraPos.x;
	//cameraTransY = playerPos.y - cameraPos.y;



    //if ((boundBottom.x < 0 && cameraTransX < 0) || (boundTop.x > gameBound.x && cameraTransX > 0 )) {
      //  cameraTransX = 0;
    //}
    
    //if ((boundTop.y < 0 && cameraTransY < 0) || (boundBottom.y > gameBound.y && cameraTransY > 0)) {
      //  cameraTransY = 0;
    //}
        
	//player->getNode()->getScene()->getCamera()->translate(cugl::Vec2(cameraTransX,cameraTransY));
}

