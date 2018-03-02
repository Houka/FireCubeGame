//
// InputController.cpp
// Coalide
//
#include "InputController.h"

using namespace cugl;


#pragma mark -
#pragma mark Constructors
/**
* Deactivates this input controller, releasing all listeners.
*
* This method will not dispose of the input controller. It can be reused
* once it is reinitialized.
*/
bool InputController::init() {
	_debugPressed = false;
	_resetPressed = false;
	_exitPressed = false;
	return true;
}

/**
* Deactivates this input controller, releasing all listeners.
*
* This method will not dispose of the input controller. It can be reused
* once it is reinitialized.
*/
void InputController::dispose() { }


#pragma mark -
#pragma mark Updating
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
void InputController::update(float dt) { }

/**
* Clears any buffered inputs so that we may start fresh.
*/
void InputController::clear() { }


#pragma mark -
#pragma mark Touch Callbacks
/**
* Callback for the beginning of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchBeganCB(const TouchEvent& event, bool focus) { }

/**
* Callback for the end of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchEndedCB(const TouchEvent& event, bool focus) { }
