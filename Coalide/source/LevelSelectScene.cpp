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
	std::shared_ptr<cugl::Node> _nextNode = PolygonNode::allocWithTexture(_assets->get<Texture>("next_button"));
	std::shared_ptr<cugl::Node> _copyOfNextNode = PolygonNode::allocWithTexture(_assets->get<Texture>("next_button"));
	std::shared_ptr<cugl::Node> _lvl0Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl1Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl2Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl3Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl4Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl5Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl6Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl7Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl8Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl9Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl10Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl11Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl12Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl13Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl14Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl15Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl16Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl17Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl18Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl19Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl20Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl21Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl22Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));
	std::shared_ptr<cugl::Node> _lvl23Node = PolygonNode::allocWithTexture(_assets->get<Texture>("immobile"));


	//_startNode->setPosition(50., 50);
	_backButton = cugl::Button::alloc(_backNode);

	_scrollForwardButton = cugl::Button::alloc(_nextNode);
	_scrollForwardButton->setScale(LOSE_WIN_SCALE);
	_scrollBackwardButton = cugl::Button::alloc(_copyOfNextNode);
	_scrollBackwardButton->setScale(LOSE_WIN_SCALE);
	_scrollBackwardButton->setAngle(M_PI);

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
	_lvl6 = cugl::Button::alloc(_lvl6Node);
	_lvl6->setScale(2, 2);
	_lvl7 = cugl::Button::alloc(_lvl7Node);
	_lvl7->setScale(2, 2);
	_lvl8 = cugl::Button::alloc(_lvl8Node);
	_lvl8->setScale(2, 2);
	_lvl9 = cugl::Button::alloc(_lvl9Node);
	_lvl9->setScale(2, 2);
	_lvl10 = cugl::Button::alloc(_lvl10Node);
	_lvl10->setScale(2, 2);
	_lvl11 = cugl::Button::alloc(_lvl11Node);
	_lvl11->setScale(2, 2);
	_lvl12 = cugl::Button::alloc(_lvl12Node);
	_lvl12->setScale(2, 2);
	_lvl13 = cugl::Button::alloc(_lvl13Node);
	_lvl13->setScale(2, 2);
	_lvl14 = cugl::Button::alloc(_lvl14Node);
	_lvl14->setScale(2, 2);
	_lvl15 = cugl::Button::alloc(_lvl15Node);
	_lvl15->setScale(2, 2);
	_lvl16 = cugl::Button::alloc(_lvl16Node);
	_lvl16->setScale(2, 2);
	_lvl17 = cugl::Button::alloc(_lvl17Node);
	_lvl17->setScale(2, 2);
	_lvl18 = cugl::Button::alloc(_lvl18Node);
	_lvl18->setScale(2, 2);
	_lvl19 = cugl::Button::alloc(_lvl19Node);
	_lvl19->setScale(2, 2);
	_lvl20 = cugl::Button::alloc(_lvl20Node);
	_lvl20->setScale(2, 2);
	_lvl21 = cugl::Button::alloc(_lvl21Node);
	_lvl21->setScale(2, 2);
	_lvl22 = cugl::Button::alloc(_lvl22Node);
	_lvl22->setScale(2, 2);
	_lvl23 = cugl::Button::alloc(_lvl23Node);
	_lvl23->setScale(2, 2);

	_backButton->setPosition(10, textureHeight*0.9);
	_scrollBackwardButton->setPosition(cugl::Vec2(xMax*.15, _scrollBackwardButton->getHeight()));
	_scrollForwardButton->setPosition(cugl::Vec2(xMax*.85, 0));
	_lvl0->setPosition(textureWidth*.08, textureHeight*.26);
	_lvl2->setPosition(textureWidth*.08, textureHeight*.75);
	_lvl3->setPosition(textureWidth*.20, textureHeight*.55);
	_lvl4->setPosition(textureWidth*.17, textureHeight*.20);
	_lvl5->setPosition(textureWidth*.28, textureHeight*.26);
	_lvl6->setPosition(textureWidth*.32, textureHeight*.67);
	_lvl7->setPosition(textureWidth*.42, textureHeight*.11);
	_lvl8->setPosition(textureWidth*.55, textureHeight*.05);
	_lvl9->setPosition(textureWidth*.50, textureHeight*.48);
	_lvl10->setPosition(textureWidth*.55, textureHeight*.64);
	_lvl1->setPosition(textureWidth*.06, textureHeight*.5);
	_lvl11->setPosition(textureWidth*.56, textureHeight*.5);
	_lvl12->setPosition(textureWidth*.6, textureHeight*.45);
	_lvl13->setPosition(textureWidth*.64, textureHeight*.35);
	_lvl14->setPosition(textureWidth*.73, textureHeight*.32);
	_lvl15->setPosition(textureWidth*.8, textureHeight*.24);
	_lvl16->setPosition(textureWidth*.9, textureHeight*.3);
	_lvl17->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl18->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl19->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl20->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl21->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl22->setPosition(textureWidth*.00, textureHeight*.00);
	_lvl23->setPosition(textureWidth*.00, textureHeight*.00);

	_backButton->setListener([=](const std::string& name, bool down) {
		if (down) {
		}
		else {
			_didClickBack = true;
		}
	});

	_scrollForwardButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			_levelSelectForwardScroll = true;
		}
		else {
			_levelSelectForwardScroll = false;
		}
	});

	_scrollBackwardButton->setListener([=](const std::string& name, bool down) {
		if (down) {
			_levelSelectBackwardScroll = true;
		}
		else {
			_levelSelectBackwardScroll = false;
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
	_lvl6->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 6;
			_didClickLevel = true;
		}
	});
	_lvl7->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 7;
			_didClickLevel = true;
		}
	});
	_lvl8->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 8;
			_didClickLevel = true;
		}
	});
	_lvl9->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 9;
			_didClickLevel = true;
		}
	});
	_lvl10->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 10;
			_didClickLevel = true;
		}
	});
	_lvl11->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 11;
			_didClickLevel = true;
		}
	});
	_lvl12->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 12;
			_didClickLevel = true;
		}
	});
	_lvl13->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 13;
			_didClickLevel = true;
		}
	});
	_lvl14->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 14;
			_didClickLevel = true;
		}
	});
	_lvl15->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 15;
			_didClickLevel = true;
		}
	});
	_lvl16->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 16;
			_didClickLevel = true;
		}
	});
	_lvl17->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 17;
			_didClickLevel = true;
		}
	});
	_lvl18->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 18;
			_didClickLevel = true;
		}
	});
	_lvl19->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 19;
			_didClickLevel = true;
		}
	});
	_lvl20->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 20;
			_didClickLevel = true;
		}
	});
	_lvl21->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 21;
			_didClickLevel = true;
		}
	});
	_lvl22->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 22;
			_didClickLevel = true;
		}
	});
	_lvl23->setListener([=](const std::string& name, bool down) {
		if (!down) {
			_lvlSelected = 23;
			_didClickLevel = true;
		}
	});


	_backButton->activate(4);
	_scrollForwardButton->activate(201);
	_scrollBackwardButton->activate(202);
	_lvl0->activate(100);
	_lvl1->activate(101);
	_lvl2->activate(102);
	_lvl3->activate(103);
	_lvl4->activate(104);
	_lvl5->activate(105);
	_lvl6->activate(106);
	_lvl7->activate(107);
	_lvl8->activate(108);
	_lvl9->activate(109);
	_lvl10->activate(110);
	_lvl11->activate(111);
	_lvl12->activate(112);
	_lvl13->activate(113);
	_lvl14->activate(114);
	_lvl15->activate(115);
	_lvl16->activate(116);
	_lvl17->activate(117);
	_lvl18->activate(118);
	_lvl19->activate(119);
	_lvl20->activate(120);
	_lvl21->activate(121);
	_lvl22->activate(122);
	_lvl23->activate(123);

	_rootnode->addChild(_background, UNIT_PRIORITY);
	_rootnode->addChild(_scrollBackwardButton, UNIT_PRIORITY);
	_rootnode->addChild(_scrollForwardButton, UNIT_PRIORITY);
	_background->addChild(_backButton, UNIT_PRIORITY);
	_background->addChild(_lvl0, UNIT_PRIORITY);
	_background->addChild(_lvl1, UNIT_PRIORITY);
	_background->addChild(_lvl2, UNIT_PRIORITY);
	_background->addChild(_lvl3, UNIT_PRIORITY);
	_background->addChild(_lvl4, UNIT_PRIORITY);
	_background->addChild(_lvl5, UNIT_PRIORITY);
	_background->addChild(_lvl6, UNIT_PRIORITY);
	_background->addChild(_lvl7, UNIT_PRIORITY);
	_background->addChild(_lvl8, UNIT_PRIORITY);
	_background->addChild(_lvl9, UNIT_PRIORITY);
	_background->addChild(_lvl10, UNIT_PRIORITY);
	_background->addChild(_lvl11, UNIT_PRIORITY);
	_background->addChild(_lvl12, UNIT_PRIORITY);
	_background->addChild(_lvl13, UNIT_PRIORITY);
	_background->addChild(_lvl14, UNIT_PRIORITY);
	_background->addChild(_lvl15, UNIT_PRIORITY);
	_background->addChild(_lvl16, UNIT_PRIORITY);
	_background->addChild(_lvl17, UNIT_PRIORITY);
	_background->addChild(_lvl18, UNIT_PRIORITY);
	_background->addChild(_lvl19, UNIT_PRIORITY);
	_background->addChild(_lvl20, UNIT_PRIORITY);
	_background->addChild(_lvl21, UNIT_PRIORITY);
	_background->addChild(_lvl22, UNIT_PRIORITY);
	_background->addChild(_lvl23, UNIT_PRIORITY);

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
	//_input.update(dt);

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

	// if that shitty android button is pressed then override all this beautiful code
	if (_levelSelectForwardScroll) {
		newOffset = -20;
	}
	else if (_levelSelectBackwardScroll) {
		newOffset = 20;
	}

	// clamp
	if (xPos + newOffset >= _maxX) {
		newOffset = _maxX - xPos;
	}
	if (xPos + newOffset <= _minX) {
		newOffset = _minX - xPos;
	}

	


	_background->setPositionX(xPos + newOffset);
}

