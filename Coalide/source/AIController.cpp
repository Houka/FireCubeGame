//
//	AIController.cpp
//	Coalide
//
#include "AIController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "TileModel.h"

using namespace cugl;


bool AIController::init() {
	return true;
}

void AIController::dispose() { }

/**
 * Go through each enemy in the level and if that enemy can move, return a corresponding
 * vector of "input" for each enemy. This logic simply aims at the player.
 *
 * For convenience the return type is a tuple of enemy and corresponding move
 */
std::vector<std::tuple<EnemyModel*, Vec2>> AIController::getEnemyMoves(std::shared_ptr<GameState> _gamestate) const{
    GameState* g = _gamestate.get();
    std::vector<std::tuple<EnemyModel*, Vec2>> moves;
    std::vector<std::shared_ptr<EnemyModel>> enemies = g->getEnemies();
    
    Vec2 player_pos = ((PlayerModel*)(g->getPlayer().get()))->getPosition();
    for(std::shared_ptr<EnemyModel> enemy_ptr : enemies){
        EnemyModel* enemy = enemy_ptr.get();
        if(!enemy->isRemoved() && enemy->canSling() && enemy->timeoutElapsed()){
            Vec2 enemy_pos = enemy->getPosition();
            Vec2 aim = player_pos - enemy_pos;
            aim = aim.normalize();
			Vec2 projectedLanding = enemy_pos + aim;
			/*std::shared_ptr<TileModel> tile = g->getTileBoard()[(int)projectedLanding.x][(int)projectedLanding.y];
			if (!tile->isWater()) {
				moves.push_back(std::make_tuple(enemy, aim));
			}
			else {
				CULog("dsffs");
			}*/
        }
    }
    return moves;
}
