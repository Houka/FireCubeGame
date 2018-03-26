//
// InputController.cpp
// Coalide
//
#include "InputController.h"
#include "Constants.h"

using namespace cugl;

/** Minimum mouse drag distance to count as a sling */
#define MIN_SLING_DISTANCE 1.0
/** The key for the event handlers */
#define LISTENER_KEY 1
/** Scale factor for display arrow */
#define SLING_VECTOR_SCALE 0.5


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
    bool success = true;

	Size dimen = Application::get()->getDisplaySize();

	_maxSling = std::min(dimen.width, dimen.height);
	_maxSling *= GAME_WIDTH / dimen.width;
	_maxSling /= 1.5;

    // Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    success = success && Input::activate<Mouse>();
    Mouse* mouse = Input::get<Mouse>();
    // Set pointer awareness to always so listening for drags registers
    // See addDragListener for an explanation
    mouse->setPointerAwareness(cugl::Mouse::PointerAwareness::ALWAYS);
    mouse->addPressListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseDownCB(event, clicks, focus);
    });
    mouse->addReleaseListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseUpCB(event, clicks, focus);
    });
    mouse->addMotionListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus) {
        this->mouseMovedCB(event, previous, focus);
    });
#else
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
	touch->addMotionListener(LISTENER_KEY, [=](const cugl::TouchEvent event, const Vec2& previous, bool focus) {
		this->touchMotionCB(event, focus);
	});
#endif
    _active = success;
    return success;
}

/**
* Deactivates this input controller, releasing all listeners.
*
* This method will not dispose of the input controller. It can be reused
* once it is reinitialized.
*/
void InputController::dispose() {
    if(_active){
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(LISTENER_KEY);
        mouse->removeReleaseListener(LISTENER_KEY);
        mouse->removeDragListener(LISTENER_KEY);
#else
        Input::deactivate<Accelerometer>();
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}


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
void InputController::update(float dt) { 
}

/**
* Clears any buffered inputs so that we may start fresh.
*/
void InputController::clear() { }

#pragma mark -
#pragma mark Polling

/**
 * A polling method to ask if the user entered a sling command
 *
 * @param shouldReset       If this is true then the next call to didSling
 *                          will return false until a different sling vector is input
 */
bool InputController::didSling(bool shouldReset){
    bool tmp = _didSling;
    if(shouldReset){
        _didSling = false;
    }
    return tmp;
}

/**
* A polling method to ask where the user is aiming
*/
cugl::Vec2 InputController::getCurrentAim() {
	_currentAim = _currentTouch - _initTouch;

	if (_currentAim.length() > _maxSling) {
		_currentAim.scale(1.0 / _currentAim.length() * _maxSling);
	}

	// _currentAim.scale(SLING_VECTOR_SCALE);
	return _currentAim;
}

#pragma mark -
#pragma mark Touch Callbacks
/**
* Callback for the beginning of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    _initTouch = event.position;
    _currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = true;
}

/**
* Callback for a continuing touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchMotionCB(const TouchEvent& event, bool focus) {
	_currentTouch = event.position;
	_previousTouch = event.position;
	_mousepan = true;
}


/**
* Callback for the end of a touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    _latestSling = Vec2(_initTouch.x - event.position.x, event.position.y - _initTouch.y);
    if(_latestSling.length() >= MIN_SLING_DISTANCE){
        _didSling = true;
    }

	if (_latestSling.length() > _maxSling) {
		_latestSling.scale(1.0 / _latestSling.length() * _maxSling);
	}

    _currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = false;
}

/**
 * Called when a mouse button is initially pressed
 *
 * @param  event    The event storing the mouse state
 * @param  clicks   The number of recent clicks, including this one
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    _initTouch = event.position;
    _currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = true;
}

/**
 * Called when a mouse button is released
 *
 * @param  event    The event storing the mouse state
 * @param  clicks   The number of recent clicks, including this one
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    _latestSling = Vec2(_initTouch.x - event.position.x, event.position.y - _initTouch.y);
    if(_latestSling.length() >= MIN_SLING_DISTANCE){
        _didSling = true;
    }

	if (_latestSling.length() > _maxSling) {
		_latestSling.scale(1.0 / _latestSling.length() * _maxSling);
	}
    
	_currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = false;
}

/**
 * Called when the mouse is moved while held down
 *
 * @param  event    The event storing the mouse state
 * @param  previous The previous position of the mouse
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseMovedCB(const cugl::MouseEvent& event, const Vec2& previous, bool focus) {
    if (_mousepan) {
        _currentTouch = event.position;
    }
}
