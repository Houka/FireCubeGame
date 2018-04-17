//
//	AIController.h
//	Coalide
//
#ifndef __AI_CONTROLLER_H__
#define __AI_CONTROLLER_H__
#include <cugl/cugl.h>
#include "GameState.h"

using namespace cugl;

class AIController {
protected:
	std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> _nextMoves;

public:
#pragma mark -
#pragma mark Constructors
	AIController() { }

	~AIController() { dispose(); }

	void dispose();

	bool init();
    
#pragma mark -
#pragma mark Logic
    /**
     * Go through each enemy in the level and if that enemy can move, return a corresponding
     * vector of "input" for each enemy. This logic simply aims at the player.
     *
     * For convenience the return type is a tuple of enemy and corresponding move
     */
    std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> getEnemyMoves(std::shared_ptr<GameState> _gamestate);
};
#endif /* __AI_CONTROLLER_H__ */
