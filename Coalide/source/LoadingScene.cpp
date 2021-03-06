//
//  LoadingScene.cpp
//	Coalide
//
#include "LoadingScene.h"
#include "Constants.h"


using namespace cugl;

/** The ID for the button listener */
#define LISTENER_ID 2

#pragma mark -
#pragma mark Constructors

/**
* Initializes the controller contents, making it ready for loading
*
* The constructor does not allocate any objects or memory.  This allows
* us to have a non-pointer reference to this controller, reducing our
* memory allocation.  Instead, allocation happens in this method.
*
* @param assets    The (loaded) assets for this game mode
*
* @return true if the controller is initialized properly, false otherwise.
*/
bool LoadingScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= GAME_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene::init(dimen)) {
		return false;
	}

	// IMMEDIATELY load the splash screen assets
	_assets = assets;
	_assets->loadDirectory("json/loading.json");
	auto layer = assets->get<Node>("load");

	layer->setContentSize(dimen);
	layer->doLayout(); // This rearranges the children to fit the screen
	
	_bar = std::dynamic_pointer_cast<ProgressBar>(assets->get<Node>("load_bar"));

	Application::get()->setClearColor(Color4(255, 255, 255, 255));

	addChild(layer);
	return true;
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void LoadingScene::dispose() {
	// Deactivate the button (platform dependent)
	if (isPending()) {
		_button->deactivate();
	}
	_button = nullptr;
	_bar = nullptr;
	_assets = nullptr;
	_progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring
/**
* The method called to update the game mode.
*
* This method updates the progress bar amount.
*
* @param timestep  The amount of time (in seconds) since the last frame
*/
void LoadingScene::update(float dt) {
	if (_progress < 1) {
		//_progress = _assets->progress();
		_progress = std::min(_progress + .05f, _assets->progress());
		_bar->setProgress(_progress);
	}
	else if (_progress >= 1) {
		this->_active = false;
	}
}

/**
* Returns true if loading is complete, but the player has not pressed play
*
* @return true if loading is complete, but the player has not pressed play
*/
bool LoadingScene::isPending() const {
	return _button != nullptr && _button->isVisible();
}
