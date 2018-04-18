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
	int worldH = gamestate->getBounds().size.getIHeight();
	int worldW = gamestate->getBounds().size.getIWidth();

	_bounds.size.set(worldW, worldH);

	_openArray = new bool*[worldH];
	_closedArray = new bool*[worldH];

	for (int i = 0; i < worldH; i++) {
		_openArray[i] = new bool[worldW];
		_closedArray[i] = new bool[worldW];
		for (int j = 0; j < worldW; j++) {
			_openArray[i][j] = false;
			_closedArray[i][j] = false;
		}
	}

	return true;
}

void AIController::dispose() {
	for (int i = 0; i < _bounds.size.getIHeight(); i++) {
		delete[] _openArray[i];
		delete[] _closedArray[i];
	}
	delete[] _openArray;
	delete[] _closedArray;
}

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
	V *= .5;
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
            if(gamestate->getTileBoard()[(int)floor(locy)][(int)floor(locx)]->isWater()){
                //CULog("WATER");
                return true;
            }
//            CULog("inner Loop");

        }
//        CULog("outer Loop %d", ct);
    }
    return false;
}


bool sortByScore(std::tuple<Vec2, Vec2, float> a, std::tuple<Vec2, Vec2, float> b) {
	return std::get<2>(a) > std::get<2>(b);
}


std::vector<Vec2> AIController::calculateRoute(Vec2 pos, float slingDist, Vec2 target, std::shared_ptr<GameState> gamestate) {
	int step = 0;

	_openList.push_back(std::make_tuple(pos, pos, target.distance(pos)));

	for (int i = 0; i < 12; i++) {
		if (_openList.empty()) {
			break;
		}
		if (_closedList.empty()) {
			AStar(pos, slingDist, target, pos, gamestate);
		}
		else if (std::get<0>(_closedList.back()).distance(target) < slingDist*1.5) {
			_closedList.push_back(std::make_tuple(target, std::get<0>(_closedList.back()), -1));
			break;
		}
		else {
			AStar(std::get<0>(_closedList.back()), slingDist, target, pos, gamestate);
		}
	}

	std::vector<Vec2> route;

	auto move = _closedList.back();
	route.push_back(std::get<0>(move));

	while (std::get<0>(move).x != std::get<1>(move).x && std::get<0>(move).y != std::get<1>(move).y) {
		route.push_back(std::get<1>(move));
		for (int j = 0; j < _closedList.size(); j++) {
			if ((int)std::get<0>(_closedList[j]).x == (int)std::get<1>(move).x && (int)std::get<0>(_closedList[j]).y == (int)std::get<1>(move).y) {
				move = _closedList[j];
			}
		}
	}

	_openList.clear();
	_closedList.clear();

	for (int i = 0; i < _bounds.size.getIHeight(); i++) {
		for (int j = 0; j < _bounds.size.getIWidth(); j++) {
			_openArray[i][j] = false;
			_closedArray[i][j] = false;
		}
	}

	return route;
}


void AIController::AStar(Vec2 pos, float slingDist, Vec2 target, Vec2 origin, std::shared_ptr<GameState> gamestate) {
	_closedList.push_back(_openList.back());
	_closedArray[(int)std::get<0>(_openList.back()).y][(int)std::get<0>(_openList.back()).x] = true;
	_openList.pop_back();

	for (int i = - ceil(slingDist); i < slingDist; i++) {
		int x = pos.x + i;
		for (int j = -ceil(slingDist); j < slingDist; j++) {
			int y = pos.y + j;
			float d = pos.distance(Vec2(x, y));
            // kyler - I changed this line to check if tile exists in tileboard[y][x] before accessing since I changed water to not be a tile
			if (x > 0 && x < _bounds.size.getIWidth() && y > 0 && y < _bounds.size.getIHeight()
				&& d < slingDist && !_closedArray[y][x] && !(i == 0 || j == 0) && !gamestate->getTileBoard()[y][x]->isWater() && !intersectsWater(pos, Vec2(x,y), gamestate)) {
				float h = target.distance(Vec2(x, y));
				float g = origin.distance(Vec2(x, y));
				if (!_openArray[y][x]) {
					_openList.push_back(std::make_tuple(Vec2(x, y), pos, g + h));
				}
				else {
					for (int k = 0; k < _openList.size(); i++) {
						if ((int)std::get<0>(_openList[k]).x == i && (int)std::get<0>(_openList[k]).y == j) {
							if (std::get<2>(_openList[k]) > g + h) {
								_openList[k] = std::make_tuple(Vec2(x, y), pos, g + h);
							}
						}
					}
				}
			}
		}
	}

	std::sort(_openList.begin(), _openList.end(), sortByScore);
}


std::shared_ptr<EnemyModel> shootSpore(Vec2 pos, Vec2 aim, std::shared_ptr<GameState> gamestate) {
	std::shared_ptr<EnemyModel> spore = EnemyModel::alloc(pos, UNIT_DIM);
	spore->setTextureKey(MUSHROOM);
	spore->setSpore();
	spore->setLinearDamping(0);

	b2Filter filter;
	filter.categoryBits = CATEGORY_SPORE;
	filter.maskBits = ~CATEGORY_MUSHROOM;
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

        if(!enemy->isRemoved() && !enemy->isStunned() && !enemy->isMushroom() && enemy->timeoutElapsed()){
			Vec2 enemy_pos = enemy->getPosition();
			Vec2 aim = player_pos - enemy_pos;

			float impulse = MAX_IMPULSE;

			if (intersectsWater(enemy_pos, player_pos, gamestate)) {
				int p = enemy->getDensity();
				int A = enemy->getWidth()*enemy->getHeight();
				int m = p*A;

				float vi = MAX_IMPULSE * 2 / m;
				float vf = 2;

				float f = GLOBAL_AIR_DRAG / m;
				/*float f1 = GLOBAL_AIR_DRAG / m;
				float f2 = gamestate->getBoard()[(int)enemy_pos.y][(int)enemy_pos.x];

				float d1 = (vi*vi - vf*vf) / (2 * f1 / m);
				float d2;
				if (f2 != 0) {
					d2 = vf*vf / (2 * f2 / m);
				}
				else {
					d2 = 0;
				}

				float d = d1 + d2;*/

				float d = vi*vi / (2 * f / m);

				if (enemy->getRoute().size() == 0) {
					enemy->setRoute(calculateRoute(enemy_pos, d, player_pos, gamestate));
				}

				std::vector<Vec2> route = enemy->getRoute();
				if (enemy->isOnion()) {
					CULog("A");
				}
				aim = route.back() + Vec2(.5,.5) - enemy_pos;
				route.pop_back();
				enemy->setRoute(route);

				float targetDist = aim.length();
				float a = GLOBAL_AIR_DRAG / m;
				float v0 = sqrt(targetDist * 2 * a);
				impulse = m * v0 / 2;
				impulse = std::min(MAX_IMPULSE, impulse);

				/*Vec2 projectedLanding = enemy_pos + aim * 3;
				Vec2 avoidance = avoidCollisions(enemy_pos, projectedLanding, gamestate);
				aim += avoidance;
				aim += flock(enemy, gamestate);*/
			}


            /*if(intersectsWater(enemy_pos, player_pos, gamestate)){
                enemy->setWaterBetween(true);
                continue;
            }*/

			aim = aim.normalize()*impulse;

			if (!intersectsWater(enemy_pos, enemy_pos + aim*1.5, gamestate)) {
				enemy->setWaterBetween(false);
				moves.push_back(std::make_tuple(enemy, aim));
			}

			/*enemy->setWaterBetween(false);
			moves.push_back(std::make_tuple(enemy, aim));*/
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
