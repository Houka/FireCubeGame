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
	* Callback for the end of a touch event
	*
	* @param t     The touch information
	* @param event The associated event
	*/
	void touchEndedCB(const cugl::TouchEvent& event, bool focus);

};

#endif /* __INPUT_CONTROLLER_H__ */
