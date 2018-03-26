//
//	AIController.cpp
//	Coalide
//
#include "AIController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"

using namespace cugl;


bool AIController::init() {
	return true;
}

void AIController::dispose() { }

bool intersectsWater(Vec2 start, Vec2 direction){
    return false;
}

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
        if(!enemy->isRemoved() && !enemy->isStunned() && enemy->canSling() && enemy->timeoutElapsed()){
            Vec2 enemy_pos = enemy->getPosition();
            Vec2 aim = player_pos - enemy_pos;
            aim = aim.normalize();
            if(intersectsWater(enemy_pos, player_pos)){
                continue;
            }
            moves.push_back(std::make_tuple(enemy, aim));
        }
    }
    return moves;
}
