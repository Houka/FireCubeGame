//
//	AIController.h
//	Coalide
//
#ifndef __AI_CONTROLLER_H__
#define __AI_CONTROLLER_H__
#include <cugl/cugl.h>

using namespace cugl;


class AIController {
public:
#pragma mark -
#pragma mark Constructors
	AIController() { }

	~AIController() { dispose(); }

	void dispose();

	bool init();
};
#endif /* __AI_CONTROLLER_H__ */
