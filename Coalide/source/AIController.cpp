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

bool intersectsWater(Vec2 start, Vec2 end, GameState* _gamestate){
    int h = _gamestate->getBounds().size.getIHeight();
    int w = _gamestate->getBounds().size.getIWidth();
    float dx = (end.x - start.x) / 20;
    float dy = (end.y - start.y) / 20;
    float locx = start.x;
    int ct = 0;
    while(locx < w && locx > 0 && ((locx > (end.x + dx) && dx < 0) || (locx < (end.x - dx) && dx > 0))){
        ct++;
        locx += dx;
        float locy = start.y;
        while(locy < h && locy > 0 && ((locy > (end.y + dy) && dy < 0) || (locy < (end.y - dy) && dy > 0))){
            locy += dy;
            int friction = _gamestate->getBoard()[(int)floor(locy)][(int)floor(locx)];
            if(friction == 0){
//                CULog("WATER");
                return true;
            }
//            CULog("inner Loop");

        }
//        CULog("outer Loop %d", ct);
    }
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
        if(!enemy->isRemoved() && !enemy->isStunned() && enemy->canSling()){
            Vec2 enemy_pos = enemy->getPosition();
            Vec2 aim = player_pos - enemy_pos;
            
			/*aim = aim.normalize()*1.25;
			Vec2 projectedLanding1 = enemy_pos + aim*.8;
			Vec2 projectedLanding2;
			Vec2 projectedLanding3;
			if (g->getTileBoard()[(int)enemy_pos.y][(int)enemy_pos.x]->getType() == TILE_TYPE::ICE) {
				 projectedLanding2 = enemy_pos + aim * 2;
				 projectedLanding3 = enemy_pos + aim * 3;
			}
			else {
				projectedLanding2 = enemy_pos + aim * 1.25;
				projectedLanding3 = enemy_pos + aim * 1.5;
			}
			std::shared_ptr<TileModel> tile1 = g->getTileBoard()[(int)projectedLanding1.y][(int)projectedLanding1.x];
			std::shared_ptr<TileModel> tile2 = g->getTileBoard()[(int)projectedLanding2.y][(int)projectedLanding2.x];
			std::shared_ptr<TileModel> tile3 = g->getTileBoard()[(int)projectedLanding3.y][(int)projectedLanding3.x];
			if (!(tile1->isWater() || tile2->isWater() || tile3->isWater())) {
				moves.push_back(std::make_tuple(enemy, aim));
			}*/

            aim = aim.normalize();
            if(intersectsWater(enemy_pos, player_pos, g)){
                enemy->setWaterBetween(true);
                continue;
            }
            enemy->setWaterBetween(false);
            if(enemy->timeoutElapsed())
                moves.push_back(std::make_tuple(enemy, aim));
        }
    }
    return moves;
}
