//
//	CoalideApp.h
//	Coalide
//
#ifndef __COALIDE_APP_H__
#define __COALIDE_APP_H__
#include <cugl/cugl.h>
#include "InputController.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "LevelSelectScene.h"

/**
 * Class for a simple Hello World style application
 *
 * The application simply moves the CUGL logo across the screen.  It also
 * provides a button to quit the application.
 */
class CoalideApp : public cugl::Application {
protected:
    /** The loaders to (synchronously) load in assets */
    std::shared_ptr<cugl::AssetManager> _assets;

    /** A 3152 style SpriteBatch to render the scene */
    std::shared_ptr<cugl::SpriteBatch>  _batch;

	// Player modes
	/** The primary controller for the game world */
	GameScene _gameScene;
	/** The controller for the loading screen */
	LoadingScene _loadingScene;
	/** The controller for the loading screen */
	MenuScene _menuScene;
	/** Controller for abstracting out input */
	LevelSelectScene _levelSelectScene;
	/** Controller for abstracting out input */
	InputController _input;

	enum class CURRENT_SCENE : int {
		MENU_SCENE,
		GAME_SCENE,
		LEVEL_SELECT_SCENE
	};


	CURRENT_SCENE _currentScene;

	/** Whether or not we have finished loading all assets */
	bool _loaded;
    
    int _levelCt = 0;
    int _numLevels = 6;

	/** Source for audio controller */
	std::shared_ptr<cugl::Music> _source;
    std::shared_ptr<cugl::Sound> _thud;

    /** 
     * Activates the correct scene.
     *
     */
    void activateGameScene();
    
public:
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
	CoalideApp() : Application() {}
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits. 
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~CoalideApp() { }
    
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
    virtual void onStartup() override;

	virtual void onSuspend() override;

	virtual void onResume() override;

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
    virtual void onShutdown() override;
    
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
    virtual void update(float timestep) override;
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void draw() override;
    
};

#endif /* __HELLO_APP_H__ */
