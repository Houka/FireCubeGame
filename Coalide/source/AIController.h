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

	bool** _openArray;
	bool** _closedArray;

	// pos, parentPos, g, h
	std::vector<std::tuple<Vec2, Vec2, float>> _openList;
	// pos, parentPos
	std::vector<std::tuple<Vec2, Vec2, float>> _closedList;

public:
#pragma mark -
#pragma mark Constructors
	AIController() { }

	~AIController() { dispose(); }

	void dispose();

	bool init(std::shared_ptr<GameState> gamestate);
    
#pragma mark -
#pragma mark Logic
    /**
     * Go through each enemy in the level and if that enemy can move, return a corresponding
     * vector of "input" for each enemy. This logic simply aims at the player.
     *
     * For convenience the return type is a tuple of enemy and corresponding move
     */
    std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> getEnemyMoves(std::shared_ptr<GameState> _gamestate);

	void AStar(Vec2 pos, float slingDist, int g);

	std::vector<Vec2> calculateRoute(Vec2 pos, Vec2 aim, Vec2 target, std::shared_ptr<GameState> gamestate);
};
#endif /* __AI_CONTROLLER_H__ */
