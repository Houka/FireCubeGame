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

bool intersectsWater(Vec2 start, Vec2 end, std::shared_ptr<GameState> gamestate){
	int h = gamestate->getBounds().size.getIHeight();
	int w = gamestate->getBounds().size.getIWidth();

	Vec2 trajectory = end - start;
	for (int i = 1; i <= 50; i++) {
		Vec2 pt = start + trajectory / 50 * i;
		if (pt.x < w-1 && pt.x > 0 && pt.y < h-1 && pt.y > 0) {
			if (!gamestate->getTileBoard()[(int)floor(pt.y)][(int)floor(pt.x)]) {
				return true;
			}
		}
	}

    return false;
}

bool AIController::slipperySlope(Vec2 landing, Vec2 aim, std::shared_ptr<EnemyModel> enemy, std::shared_ptr<GameState> gamestate) {
	if (landing.x < 0 || landing.x >= _bounds.size.getIWidth() || landing.y < 0 || landing.y >= _bounds.size.getIHeight()) {
		return true;
	}
	
	float friction = gamestate->getBoard()[(int)landing.y][(int)landing.x];

	if (friction == 0) {
		return true;
	}

	float m = enemy->getMass();
	float vi = MIN_SPEED_FOR_CHARGING;

	float a = friction / m;
	float d = (vi*vi) / (2 * a);
	Vec2 slide = landing + aim*d*1.8;
	Vec2 shortland = landing - aim*d*0.8;
	if (slide.x < 0 || slide.x >= _bounds.size.getIWidth() || slide.y < 0 || slide.y >= _bounds.size.getIHeight() || !gamestate->getTileBoard()[(int)slide.y][(int)slide.x] || intersectsWater(shortland, slide, gamestate)) {
		return true;
	}
	return false;
}


bool sortByScore(std::tuple<Vec2, Vec2, float> a, std::tuple<Vec2, Vec2, float> b) {
	return std::get<2>(a) > std::get<2>(b);
}


std::vector<Vec2> AIController::calculateRoute(Vec2 pos, float slingDist, Vec2 target, std::shared_ptr<EnemyModel> enemy, std::shared_ptr<GameState> gamestate) {
	int step = 0;

	_openList.push_back(std::make_tuple(pos, pos, target.distance(pos)));

	AStar(pos, slingDist, target, pos, enemy, gamestate);

	for (int i = 0; i < 24; i++) {
		if (_openList.empty()) {
			break;
		}
		else if (!intersectsWater(std::get<0>(_closedList.back()), target, gamestate)) {
			break;
		}
		else {
			AStar(std::get<0>(_closedList.back()), slingDist, target, pos, enemy, gamestate);
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
				break;
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


void AIController::AStar(Vec2 pos, float slingDist, Vec2 target, Vec2 origin, std::shared_ptr<EnemyModel> enemy, std::shared_ptr<GameState> gamestate) {
	_closedList.push_back(_openList.back());
	_closedArray[(int)std::get<0>(_openList.back()).y][(int)std::get<0>(_openList.back()).x] = true;
	_openList.pop_back();

	for (int i = -ceil(slingDist); i < slingDist; i++) {
		int xi = (int)floor(pos.x + i);
		for (int j = -ceil(slingDist); j < slingDist; j++) {
			int yi = (int)floor(pos.y + j);
			Vec2 aim = Vec2(xi + .5, yi + .5) - pos;
			aim.normalize();
			Vec2 vec = pos + aim * slingDist;
			//Vec2 vec = Vec2(xi + .5, yi + .5);
			int x = (int)floor(vec.x);
			int y = (int)floor(vec.y);

			if (x >= 0 && x < _bounds.size.getIWidth() && y >= 0 && y < _bounds.size.getIHeight()
				&& !_closedArray[y][x] && !(i == 0 && j == 0) && gamestate->getTileBoard()[y][x] && !slipperySlope(vec, aim, enemy, gamestate)) {
				float h = target.distance(vec);
				float g = origin.distance(vec);
				if (!_openArray[y][x]) {
					_openList.push_back(std::make_tuple(vec, pos, g + h));
				}
				else {
					for (int k = 0; k < _openList.size(); i++) {
						if ((int)std::get<0>(_openList[k]).x == i && (int)std::get<0>(_openList[k]).y == j) {
							if (std::get<2>(_openList[k]) > g + h) {
								_openList[k] = std::make_tuple(vec, pos, g + h);
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
	spore->setTextureKey(SPORE);
	spore->setSpore();
	spore->setLinearDamping(0);
	spore->setDensity(6);

	b2Filter filter;
	filter.categoryBits = CATEGORY_SPORE;
	filter.maskBits = ~CATEGORY_MUSHROOM;
	filter.groupIndex = NULL;
	spore->setFilterData(filter);

	spore->setShooting();

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
		int enemyRange = 12;
		if (enemy->isAcorn()) {
			enemyRange = 7;
		}
		if (!enemy->didFall() && !enemy->isRemoved() && !enemy->isStunned() && enemy->canSling() && enemy->isTargeting() && gamestate->getPlayer()->getLinearVelocity().length() < 1 && player_pos.distance(enemy_pos) < enemyRange) {
			if (enemy->timeoutElapsed() && !enemy->isPrepping()) {
				enemy->setPrepping(true);
			}
			else if (enemy->isPrepping()) {
				enemy->_prepTimer -= 1;
				if (enemy->_prepTimer == 0) {
					enemy->setPrepping(false);
				}
			}
			if (!enemy->isMushroom() && enemy->timeoutElapsed() && !gamestate->getPlayer()->getCharging()) {
				Vec2 aim = player_pos - enemy_pos;
				aim.normalize();
				float impulse = MAX_IMPULSE;

				int m = enemy->getMass();
				float a = GLOBAL_AIR_DRAG * 7.45;
				float vi = MAX_IMPULSE;
				float vf = MIN_SPEED_FOR_CHARGING;
				float d = ((vi*vi) - (vf*vf)) / (2 * a);

				/*if (intersectsWater(enemy_pos, player_pos, gamestate)) {
					if (enemy->getRoute().empty()) {
						enemy->setRoute(calculateRoute(enemy_pos, d, player_pos, enemy, gamestate));
					}

					std::vector<Vec2> route = enemy->getRoute();
					aim = route.back() - enemy_pos;
					route.pop_back();
					enemy->setRoute(route);
				}*/

				aim.normalize();

				/*if (!slipperySlope(enemy_pos+aim, aim, enemy, gamestate)) {
					moves.push_back(std::make_tuple(enemy, aim));
				}
				*/
				if (intersectsWater(enemy_pos, player_pos, gamestate)) {
					if (!slipperySlope(enemy_pos + aim*d, aim, enemy, gamestate)) {
						aim *= impulse;
						if (!enemy->isPrepping()) {
							moves.push_back(std::make_tuple(enemy, aim));
						}
					}
					else {
						float theta = 3.14159 / 8;
						float rotated = 0;
						Vec2 rotatedAim1 = aim;
						Vec2 rotatedAim2 = aim;
						bool success = false;
						while (abs(rotated) < 3.14159) {
							rotatedAim1 = Vec2(rotatedAim1.x*cos(theta) - rotatedAim1.y*sin(theta), rotatedAim1.x*sin(theta) + rotatedAim1.y*cos(theta));
							rotatedAim1.normalize();
							rotatedAim2 = Vec2(rotatedAim2.x*cos(-theta) - rotatedAim2.y*sin(-theta), rotatedAim2.x*sin(-theta) + rotatedAim2.y*cos(-theta));
							rotatedAim2.normalize();
							if (!slipperySlope(enemy_pos + rotatedAim1*d, rotatedAim1, enemy, gamestate)) {
								rotatedAim1 *= impulse;
								if (!enemy->isPrepping()) {
									moves.push_back(std::make_tuple(enemy, rotatedAim1));
								}
								success = true;
								break;
							}
							else if (!slipperySlope(enemy_pos + rotatedAim2*d, rotatedAim2, enemy, gamestate)) {
								rotatedAim2 *= impulse;
								if (!enemy->isPrepping()) {
									moves.push_back(std::make_tuple(enemy, rotatedAim2));
								}
								success = true;
								break;
							}
							rotated += theta;
						}
						if (!success) {
							enemy->setPrepping(false);
						}
					}
				}
				else {
					aim *= impulse;
					if (!enemy->isPrepping()) {
						moves.push_back(std::make_tuple(enemy, aim));
					}
				}
			}
			else if (enemy->isMushroom() && !enemy->isRemoved() && !enemy->isStunned()) {
				if (enemy->timeoutElapsed() && !enemy->isPrepping()) {
					moves.push_back(std::make_tuple(enemy, Vec2(0, 0)));
					Vec2 aim = player_pos - enemy->getPosition();
					aim.normalize();
					aim *= MAX_IMPULSE;
					std::shared_ptr<EnemyModel> spore = shootSpore(enemy->getPosition(), aim, gamestate);
					std::tuple<std::shared_ptr<EnemyModel>, Vec2> nextMove = std::make_tuple(spore, aim / 2);
					_nextMoves.push_back(nextMove);
				}
			}
		}
		else {
			//Wandering
		}
    }
    return moves;
}
