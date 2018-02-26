//
//  JSWallModel.cpp
//  JSONDemo
//
//  This encapsulates all of the information for representing a wall. Since wall never
//  move, we normally would not need a separate class (because we could just use an instance
//  of PolygonObstacle).  However, this changes when we do dynamic level loading.
//
//  The problem is that we need to initialize an physics object when a level is loaded,
//  but NOT attach any assets yet.  That is because the physics object is itself an asset
//  and it is not safe to assume the order of asset loading.  So instead, we just store
//  the keys of the assets in this class, and they attach the assets later when we
//  initialize the scene graph.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/20/16
//
#include "TileModel.h"

#pragma mark -
#pragma mark Initializers

/**
* Initializes a wall from (not necessarily convex) polygon
*
* The anchor point (the rotational center) of the polygon is specified as a
* ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
* the bounding box.  An anchor point of (1,1) is the top right of the bounding
* box.  The anchor point does not need to be contained with the bounding box.
*
* @param  poly     The polygon vertices
* @param  anchor   The rotational center of the polygon
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool TileModel::init(const Vec2& pos, const Size& size) {
	if (BoxObstacle::init(pos, size)) {
		std::string name("tile");
		setName(name);
		_tileTexture = "";
		return true;
	}
	return false;
}
