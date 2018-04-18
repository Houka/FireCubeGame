//
//	AIController.cpp
//	Coalide
//
#include "AIController.h"
#include "GameState.h"
#include "PlayerModel.h"
#include "EnemyModel.h"
#include "ObjectModel.h"
#include "TileModel.h"

using namespace cugl;


bool AIController::init(std::shared_ptr<GameState> gamestate) {
	/*int worldH = gamestate->getBounds().size.getIHeight();
	int worldW = gamestate->getBounds().size.getIWidth();

	_openArray = new bool*[worldH];
	_closedArray = new bool*[worldH];

	for (int i = 0; i < worldH; i++) {
		_openArray[i] = new bool[worldW];
		_closedArray[i] = new bool[worldW];
		for (int j = 0; j < worldW; j++) {
			_openArray[i][j] = false;
			_closedArray[i][j] = false;
		}
	}*/

	return true;
}

void AIController::dispose() { }

Vec2 avoidCollisions(Vec2 start, Vec2 end, std::shared_ptr<GameState> gamestate) {
	Vec2 d = end - start;
	std::vector<std::shared_ptr<ObjectModel>>& objects = gamestate->getObjects();
	std::shared_ptr<ObjectModel> nearest;

	for (int i = 0; i < gamestate->getObjects().size(); i++) {
		std::shared_ptr<ObjectModel> obj = objects[i];
		Vec2 e = obj->getPosition();
		Vec2 f = start - e;
		if (f.length() < d.length()) {
			float x = e.x;
			float y = e.y;
			
			float a = d.dot(d);
			float b = 2 * f.dot(d);
			float c = f.dot(f) - (UNIT_DIM.x+.5) * (UNIT_DIM.x+.5);

			float discriminant = b*b - 4*a*c;

			if (discriminant < 0) {
				return Vec2();
			}
			else {
				discriminant = sqrt(discriminant);

				float t1 = (-b - discriminant) / (2 * a);
				float t2 = (-b + discriminant) / (2 * a);

				if ((t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1)) {
					float aa = end.y - start.y;
					float bb = end.x - start.x;
					float cc = start.y + (a / b)*start.x;

					Vec2 closestPt = Vec2(-(aa*cc) / (aa*aa + bb*bb), -(bb*cc) / (aa*aa + bb*bb));

					float forceScale = 5 - (f.length() / d.length());

					Vec2 avoidance = end - e;
					avoidance.normalize();
					avoidance *= forceScale;
					return avoidance;
				}
				else {
					return Vec2();
				}
			}
		}
	}
	return Vec2();
}

Vec2 flock(std::shared_ptr<EnemyModel> enemy, std::shared_ptr<GameState> gamestate) {
	std::vector<std::shared_ptr<EnemyModel>>& enemies = gamestate->getEnemies();
	int nFlock = 0;
	int nRepel = 0;
	Vec2 flockV = Vec2();
	Vec2 repelV = Vec2();
	for (int i = 0; i < enemies.size(); i++) {
		std::shared_ptr<EnemyModel> fellow = enemies[i];
		float dist = fellow->getPosition().distance(enemy->getPosition());
		if (fellow->isTargeting()) {
			if (dist < 6 && dist >= 3) {
				nFlock += 1;
				flockV += fellow->getPosition() - enemy->getPosition();
			}
			else if (dist < 3 && dist > .1) {
				nRepel += 1;
				repelV += fellow->getPosition() - enemy->getPosition();
			}
		}
	}
	if (nFlock > 0) {
		flockV /= nFlock;
		flockV.normalize();
		flockV *= .4;
	}

	if (nRepel > 0) {
		repelV /= nRepel;
		repelV *= -1;
		repelV.normalize();
		repelV *= .2;
	}

	Vec2 V = flockV + repelV;
	V.normalize();
	V *= .8;
	return V;
}

bool intersectsWater(Vec2 start, Vec2 end, std::shared_ptr<GameState> gamestate){
    int h = gamestate->getBounds().size.getIHeight();
    int w = gamestate->getBounds().size.getIWidth();
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
            int friction = gamestate->getBoard()[(int)floor(locy)][(int)floor(locx)];
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


//bool sortByScore(std::tuple<Vec2, Vec2, float> a, std::tuple<Vec2, Vec2, float> b) {
//	return std::get<2>(a) > std::get<2>(b);
//}
//
//
//void AIController::AStar(Vec2 pos, float slingDist, int g) {
//	_closedList.push_back(_openList[0]);
//	_openList.pop_back();
//
//	for (int i = 0; i < slingDist; i++) {
//		int x = pos.x + i;
//		for (int j = 0; j < slingDist; j++) {
//			int y = pos.y + j;
//			float d = pos.distance(Vec2(x, y));
//			if (d < slingDist && d > slingDist - sqrt(2) && !_closedArray[i][j]) {
//				float h = pos.distance(Vec2(x, y));
//				if (!_openArray[i][j]) {
//					_openList.push_back(std::make_tuple(Vec2(i, j), pos, g + h));
//				}
//				else {
//					for (int i = 0; i < _openList.size(); i++) {
//						if (std::get<0>(_openList[i]).x == i && std::get<0>(_openList[i]).y == j) {
//							if (std::get<2>(_openList[i]) < g + h) {
//								_openList[i] = std::make_tuple(Vec2(i, j), pos, g + h);
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//	std::sort(_openList.begin(), _openList.end(), sortByScore);
//}
//
//
//std::vector<Vec2> AIController::calculateRoute(Vec2 pos, Vec2 aim, Vec2 target, std::shared_ptr<GameState> gamestate) {
//	float slingDist = aim.length();
//	int step = 0;
//
//	_openList.push_back(std::make_tuple(pos, pos, target.distance(pos)));
//
//	for (int i = 0; i < 5; i++) {
//		AStar(pos, slingDist, i+1);
//	}
//
//	std::vector<Vec2> route;
//
//	auto move = _closedList[_closedList.size() - 1];
//	route.push_back(std::get<0>(move));
//
//	for (int i = 0; i < 4; i++) {
//		route.push_back(std::get<1>(move));
//		for (int j = 0; j < _openList.size(); j++) {
//			if (std::get<0>(_openList[j]).x == std::get<1>(move).x && std::get<0>(_openList[j]).y == std::get<1>(move).y) {
//				move = _openList[j];
//			}
//		}
//	}
//
//	route.push_back(std::get<1>(move));
//
//	return route;
//}


std::shared_ptr<EnemyModel> shootSpore(Vec2 pos, Vec2 aim, std::shared_ptr<GameState> gamestate) {
	std::shared_ptr<EnemyModel> spore = EnemyModel::alloc(pos, UNIT_DIM);
	spore->setTextureKey(MUSHROOM);
	spore->setSpore();
	spore->setLinearDamping(0);

	b2Filter filter;
	filter.categoryBits = CATEGORY_SPORE;
	filter.maskBits = CATEGORY_PLAYER;
	filter.groupIndex = NULL;
	spore->setFilterData(filter);

	gamestate->addSporeNode(spore);
	gamestate->getWorld()->addObstacle(spore);

	std::vector<std::shared_ptr<EnemyModel>> spores = gamestate->getSpores();
	spores.push_back(spore);
	gamestate->setSpores(spores);

	return spore;
}

/**
 * Go through each enemy in the level and if that enemy can move, return a corresponding
 * vector of "input" for each enemy. This logic simply aims at the player.
 *
 * For convenience the return type is a tuple of enemy and corresponding move
 */
std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> AIController::getEnemyMoves(std::shared_ptr<GameState> gamestate) {
    std::vector<std::tuple<std::shared_ptr<EnemyModel>, Vec2>> moves;
    std::vector<std::shared_ptr<EnemyModel>> enemies = gamestate->getEnemies();
    
    Vec2 player_pos = gamestate->getPlayer()->getPosition();

	for (int i = 0; i < _nextMoves.size(); i++) {
		moves.push_back(_nextMoves[i]);
	}
	_nextMoves.clear();

    for(std::shared_ptr<EnemyModel> enemy_ptr : enemies){
		std::shared_ptr<EnemyModel> enemy = enemy_ptr;

		Vec2 enemy_pos = enemy->getPosition();
		Vec2 aim = player_pos - enemy_pos;

		aim = aim.normalize();

		/*if (enemy->getRoute().size() == 0) {
			enemy->setRoute(calculateRoute(enemy_pos, aim, player_pos, gamestate));
		}*/

        if(!enemy->isRemoved() && !enemy->isStunned() && !enemy->isMushroom()){
			/*std::vector<Vec2> route = enemy->getRoute();
			aim = route[route.size()];
			route.pop_back();
			enemy->setRoute(route);*/

			Vec2 projectedLanding = enemy_pos + aim*3;

			Vec2 avoidance = avoidCollisions(enemy_pos, projectedLanding, gamestate);
			//aim += avoidCollisions(enemy_pos, projectedLanding, g);
			aim += avoidance;

			if (enemy->isTargeting()) {
				aim += flock(enemy, gamestate);
			}

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

            aim = aim.normalize()*1.5;

            if(intersectsWater(enemy_pos, player_pos, gamestate)){
                enemy->setWaterBetween(true);
                continue;
            }
            enemy->setWaterBetween(false);
			if (enemy->timeoutElapsed())
				moves.push_back(std::make_tuple(enemy, aim));
        }
		else if(enemy->isMushroom() && !enemy->isRemoved() && !enemy->isStunned()) {
			if (enemy->timeoutElapsed()) {
				moves.push_back(std::make_tuple(enemy, Vec2(0,0)));
				Vec2 aim = player_pos - enemy->getPosition();
				aim.normalize();
				std::shared_ptr<EnemyModel> spore = shootSpore(enemy->getPosition(), aim, gamestate);
				std::tuple<std::shared_ptr<EnemyModel>, Vec2> nextMove = std::make_tuple(spore, aim);
				_nextMoves.push_back(nextMove);
			}
		}
    }
    return moves;
}
