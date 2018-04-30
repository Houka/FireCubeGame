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
	_mousepan = false;
    //_mousedown = false;

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
    mouse->addDragListener(LISTENER_KEY, [=](const cugl::MouseEvent& event, const cugl::Vec2& previous, bool focus) {
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
        touch->removeMotionListener(LISTENER_KEY);
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
    
    Keyboard* keys = Input::get<Keyboard>();
    
    // Map "keyboard" events to the current frame boundary
    _left  = keys->keyPressed(KeyCode::ARROW_LEFT);
    _right = keys->keyPressed(KeyCode::ARROW_RIGHT);
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
//    CULog("(%f,%f)", _initTouch.x, _initTouch.y);

	if (_currentAim.length() > _maxSling) {
		_currentAim.scale(1.0 / _currentAim.length() * _maxSling);
	}

	CULog("INIT TOUCH: %f", _initTouch);
	CULog("CURRENT TOUCH: %f", _currentTouch);

	// _currentAim.scale(SLING_VECTOR_SCALE);
	//CULog("(%f,%f)", _currentAim.x, _currentAim.y);
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
    CULog("touch began");
    _initTouch = event.position;
    _currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = true;
    _mousedown = true;
}

/**
* Callback for a continuing touch event
*
* @param t     The touch information
* @param event The associated event
*/
void InputController::touchMotionCB(const TouchEvent& event, bool focus) {
    CULog("touch motion");
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
    CULog("touch ended");
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
    _mousedown = false;
}

/**
 * Called when a mouse button is initially pressed
 *
 * @param  event    The event storing the mouse state
 * @param  clicks   The number of recent clicks, including this one
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//    CULog("mouse down");
    _initTouch = event.position;
    _currentTouch = event.position;
    _previousTouch = event.position;
    _mousepan = true;
//    CULog("down: %s", _mousedown ? "true" : "false");
    _mousedown = true;
}

/**
 * Called when a mouse button is released
 *
 * @param  event    The event storing the mouse state
 * @param  clicks   The number of recent clicks, including this one
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
//    CULog("mouse up");
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
//    CULog("down: %s", _mousedown ? "true" : "false");
    _mousedown = false;
}

/**
 * Called when the mouse is moved while held down
 *
 * @param  event    The event storing the mouse state
 * @param  previous The previous position of the mouse
 * @parm   focus       Whether the listener currently has focus
 */
void InputController::mouseMovedCB(const cugl::MouseEvent& event, const Vec2& previous, bool focus) {
    //CULog("mouse moved");
    CULog("down: %s", _mousedown ? "true" : "false");
    if (_mousedown) {
//        CULog("mousepan true");
        _currentTouch = event.position;
    }
}

