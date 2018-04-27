//
//  CoalideApp.cpp
//  Coalide
//
// Include the class header, which includes all of the CUGL classes
#include "CoalideApp.h"
#include <cugl/base/CUBase.h>
#include "Constants.h"
#include "LevelController.h"


// This keeps us from having to write cugl:: all the time
using namespace cugl;


/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void CoalideApp::onStartup() {
	// Activate mouse or touch screen input as appropriate
	// We have to do this BEFORE the scene, because the scene has a button
#if defined (CU_TOUCH_SCREEN)
	Input::activate<Touchscreen>();
#else
	Input::activate<Mouse>();
#endif
	Input::activate<Keyboard>();
    // Create a sprite batch (and background color) to render the scene
    _batch = SpriteBatch::alloc();
    setClearColor(Color4(229,229,229,255));
    
    // Create an asset manager to load all assets
    _assets = AssetManager::alloc();
    
    // You have to attach the individual loaders for each asset type
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
	_assets->attach<Node>(SceneLoader::alloc()->getHook());
	_assets->attach<LevelController>(GenericLoader<LevelController>::alloc()->getHook());
	_assets->attach<Sound>(SoundLoader::alloc()->getHook());
	AudioEngine::start();

    // This reads the given JSON file and uses it to load all other assets
    _assets->loadDirectory("json/assets.json");

	// Load the level
	_assets->loadAsync<LevelController>(LEVEL_KEY, LEVEL_FILE, nullptr);

	// Create a "loading" screen
	_loaded = false;
	_loadingScene.init(_assets);

    Application::onStartup();
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void CoalideApp::onShutdown() {
    // Delete all smart pointers
	_loadingScene.dispose();
	_gameScene.dispose();
    _batch = nullptr;
    _assets = nullptr;
    
    // Deativate input
#if defined CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Application::onShutdown();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void CoalideApp::update(float timestep) {
    //std::string levelNames[5] = {"json/demo/enemy_no_water.json", "json/demo/map.json", "json/demo/island_all_enemies.json", "json/demo/3.json", "json/demo/enemy_water.json"};
    std::string levelNames[6] = {"json/updatedJsons/nicoalonly.json", "json/updatedJsons/rings.json", "json/updatedJsons/ishape.json", "json/updatedJsons/large.json", "json/updatedJsons/onionfight.json", "json/updatedJsons/icebridge.json"};
	
	if (!_loaded && _loadingScene.isActive()) {
		_loadingScene.update(0.01f);
	}
	else if (!_loaded) {
		_loadingScene.dispose(); // Disables the input listeners in this mode
		//_gameScene.init(_assets, _input, LEVEL_KEY);
		_menuScene.init(_assets, _input);
		_currentScene = CURRENT_SCENE::MENU_SCENE;

		if (!AudioEngine::get()->isActiveEffect("harlem")) {
			auto source = _assets->get<Sound>("harlem");
			//AudioEngine::get()->playEffect("harlem", source, true, source->getVolume());
		}
		// _levelSelectScene.init(_assets, _input, LEVEL_KEY);
		_loaded = true;
	}
	else {
		_input.update(timestep);
		if (_currentScene == CURRENT_SCENE::MENU_SCENE) {
			//CULog("currently menu scene");
			if (_menuScene.didClickStart()) {
				_menuScene.dispose();
				_gameScene.init(_assets, _input, LEVEL_KEY);
				_currentScene = CURRENT_SCENE::GAME_SCENE;
			}
			else if (_menuScene.didClickLevels()) {
				_menuScene.dispose();
				_levelSelectScene.init(_assets, _input);
				_currentScene = CURRENT_SCENE::LEVEL_SELECT_SCENE;
			}
			else {
				_menuScene.update(timestep);
			}
		}
		if (_currentScene == CURRENT_SCENE::LEVEL_SELECT_SCENE) {
			//CULog("currently level select scene");
			if (_levelSelectScene.didClickBack()) {
				_levelSelectScene.dispose();
				_menuScene.init(_assets, _input);
				_currentScene = CURRENT_SCENE::MENU_SCENE;
			}
			else if (_levelSelectScene.didClickLevel()) {
				_levelSelectScene.dispose();
				_gameScene.init(_assets, _input, LEVEL_KEY);
				_levelCt = _levelSelectScene.getCurrentLevel();
				_gameScene.reset(levelNames[_levelCt]);
				_currentScene = CURRENT_SCENE::GAME_SCENE;
			}
			else {
				_levelSelectScene.update(timestep);
			}
		}
		if (_currentScene == CURRENT_SCENE::GAME_SCENE) {
			//CULog("currently game scene");
			if (_gameScene.getGameState()->didClickMenu()) {
				_gameScene.dispose();
				_menuScene.init(_assets, _input);
				_currentScene = CURRENT_SCENE::MENU_SCENE;
			} 
			else if (_gameScene.isComplete()) {
				_levelCt = (_levelCt + 1) % 6;
				_gameScene.reset(levelNames[_levelCt]);
			}
			else if (_gameScene.isGameOver()) {
				_gameScene.reset(levelNames[_levelCt]);
			}
			else {
				_gameScene.update(timestep);
			}

			if (_input.rightKeyPressed()) {
				_levelCt = (_levelCt + 1) % 6;
				_gameScene.reset(levelNames[_levelCt]);
			}
		}	
	}
//    {
//
//        //_gameScene.reset("json/paulsmall.json");
//        //_gameScene.init(_assets, _input, "json/paulsmall.json");
//    }

//    if (_input.didSling()) {
//        // CULog("SLANG");
//        //_gameScene.init(_assets, _input, "json/paulsmall.json");
//    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void CoalideApp::draw() {
    // This takes care of begin/end
	if (!_loaded) {
		_loadingScene.render(_batch);
	}
	else {
		if (_currentScene == CURRENT_SCENE::GAME_SCENE) {
			_gameScene.render(_batch);
		}
		if (_currentScene == CURRENT_SCENE::MENU_SCENE) {
			_menuScene.render(_batch);
		}
		if (_currentScene == CURRENT_SCENE::LEVEL_SELECT_SCENE) {
			_levelSelectScene.render(_batch);
		}
	}
}

/**
 * Internal helper to build the scene graph.
 *
 * Scene graphs are not required.  You could manage all scenes just like
 * you do in 3152.  However, they greatly simplify scene management, and
 * have become standard in most game engines.
 */
void CoalideApp::activateGameScene() { }
