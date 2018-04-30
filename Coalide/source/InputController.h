//
//	InputController.h
//	Coalide
//
#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__
#include <cugl/cugl.h>

class InputController {
private:
	bool _debugPressed;
	bool _resetPressed;
	bool _exitPressed;
    bool _didSling;
	bool _left;
	bool _right;
	bool _didTap;
	float _maxSling;
	/** Is this inputController active */
    bool _active = false;
    /** Whether or not are in an active mouse pan */
    bool _mousepan;
    bool _mousedown;
    // TOUCH SUPPORT
    /** The initial touch location for the current gesture */
    cugl::Vec2 _initTouch;
    /** The latest vector that represents a sling movement */
    cugl::Vec2 _latestSling;
	/** The current vector that represents the aim arrow */
	cugl::Vec2 _currentAim;
    /** The timestamp for the beginning of the current gesture */
    cugl::Timestamp _timestamp;
    /** The last touch location for the current gesture */
    cugl::Vec2 _previousTouch;
    /** The current touch location for the current gesture */
    cugl::Vec2 _currentTouch;
public:
#pragma mark -
#pragma mark Constructors
	/**
	* Creates a new input controller.
	*
	* This constructor does NOT do any initialzation.  It simply allocates the
	* object. This makes it safe to use this class without a pointer.
	*/
	InputController() { }

	/**
	* Disposes of this input controller, releasing all listeners.
	*/
	~InputController() { dispose(); }

	/**
	* Deactivates this input controller, releasing all listeners.
	*
	* This method will not dispose of the input controller. It can be reused
	* once it is reinitialized.
	*/
	void dispose();

	/**
	* Deactivates this input controller, releasing all listeners.
	*
	* This method will not dispose of the input controller. It can be reused
	* once it is reinitialized.
	*/
	bool init();

#pragma mark -
#pragma mark Input Detection
	/**
	* Processes the currently cached inputs.
	*
	* This method is used to to poll the current input state.  This will poll the
	* keyboad and accelerometer.
	*
	* This method also gathers the delta difference in the touches. Depending on
	* the OS, we may see multiple updates of the same touch in a single animation
	* frame, so we need to accumulate all of the data together.
	*/
	void update(float dt);

	/**
	* Clears any buffered inputs so that we may start fresh.
	*/
	void clear();

#pragma mark -
#pragma mark Input Results
	
	/**
	* Returns true if the player wants to go toggle the debug mode.
	*
	* @return true if the player wants to go toggle the debug mode.
	*/
	bool didDebug() const { return _debugPressed; }

	/**
	* Returns true if the player wants to reset the game.
	*
	* @return true if the player wants to reset the game.
	*/
	bool didReset() const { return _resetPressed; }

	/**
	* Returns true if the player wants to exit the game.
	*
	* @return true if the player wants to exit the game.
	*/
	bool didExit() const { return _exitPressed; }

	bool leftKeyPressed() { return _left; }
	bool rightKeyPressed() { return _right; }


#pragma mark -
#pragma mark Mouse Callbacks
    
    /**
     * Called when a mouse button is initially pressed
     *
     * This is called in addition to the touch event, as mouse events are
     * all touch events. The mousepan boolean keeps them both from
     * being processed in future events.
     *
     * @param  event    The event storing the mouse state
     * @param  clicks   The number of recent clicks, including this one
     * @parm   focus       Whether the listener currently has focus
     */
    void    mouseDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
    
    /**
     * Called when a mouse button is released
     *
     * This is called in addition to the touch event, as mouse events are
     * all touch events. The mousepan boolean keeps them both from
     * being processed in future events.
     *
     * @param  event    The event storing the mouse state
     * @param  clicks   The number of recent clicks, including this one
     * @parm   focus       Whether the listener currently has focus
     */
    void    mouseUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
    
    /**
     * Called when the mouse moves
     *
     * @param  event    The event storing the mouse state
     * @param  previous The previous position of the mouse
     * @parm   focus       Whether the listener currently has focus
     */
    void    mouseMovedCB(const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus);
    
#pragma mark -
#pragma mark Touch Callbacks
    
	/**
	* Callback for the beginning of a touch event
	*
	* @param t     The touch information
	* @param event The associated event
	*/
	void touchBeganCB(const cugl::TouchEvent& event, bool focus);

	/**
	* Callback for continuing a touch event
	*
	* @param t     The touch information
	* @param event The associated event
	*/
	void touchMotionCB(const cugl::TouchEvent& event, bool focus);

	/**
	* Callback for the end of a touch event
	*
	* @param t     The touch information
	* @param event The associated event
	*/
	void touchEndedCB(const cugl::TouchEvent& event, bool focus);
    
#pragma mark Polling
    
    /**
     * A polling method to ask if the user entered a sling command
     *
     * @param shouldReset       If this is true then the next call to didSling
     *                          will return false until a different sling vector is input
     */
    bool didSling(bool shouldReset = false);
    
    /**
     * A polling method to ask if the user started the sling command
     */
    bool didStartSling(){
        return _mousepan;
    }
    
    /**
     * A polling method return the latest sling vector
     */
    cugl::Vec2 getLatestSlingVector() const {
        return _latestSling;
    }

	/**
	* A polling method to ask where the user is aiming
	*/
	cugl::Vec2 getCurrentAim();
};

#endif /* __INPUT_CONTROLLER_H__ */
