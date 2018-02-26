#ifndef __RD_GAME_SCENE_H__
#define __RD_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <vector>
#include "RDRocketModel.h"
#include "RDInput.h"

class LevelController {
private:
	GameState _gameState;

public:
	GameState getGameState();
