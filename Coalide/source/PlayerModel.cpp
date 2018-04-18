//
// PlayerModel.cpp
// Coalide
//

#include "PlayerModel.h"
#include "Constants.h"

#define IMPULSE_SCALE .05
#define COLLISION_TIMEOUT 0
#define MAX_SPEED_FOR_SLING 2


/**
* Initializes a new player with the given position and size.
*
* @param  pos		Initial position in world coordinates
* @param  size   	The dimensions of the sprite.
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool PlayerModel::init(const Vec2 & pos, const Size & size) {
	if (CapsuleObstacle::init(pos, size, Orientation::VERTICAL)) {
		setName(PLAYER_NAME);
		setTextureKey("nicoal_nicoal_f");
		setBodyType(b2_dynamicBody);
        setLinearDamping(GLOBAL_AIR_DRAG);
        
//        _anim = AnimationNode::alloc(texture, 7, 8);
//        _anim->setAnchor(Vec2::ANCHOR_CENTER);
//        _anim->setScale(2.0f);
//        _anim->setPosition(size/2);
//        _anim->setVisible(false);

		_node = nullptr;
        _color = Color4::WHITE;
        _charging = false;
        _sizePlayer = size;

		setDensity(2.0f);
		setRestitution(0.4f);
		setFixedRotation(true);

		_stunned = false;
        _stunOnStop = false;
		_onFire = false;

		return true;
	}
	return false;
}

void PlayerModel::dispose() {
	_node = nullptr;
	_frictionJoint = nullptr;
}

void PlayerModel::switchNode(std::shared_ptr<Node> fromNode, std::shared_ptr<Node> toNode) {
    fromNode->setVisible(false);
    toNode->setVisible(true);
    _node = toNode;
}

std::shared_ptr<Node> PlayerModel::getTextNode(int state, int dir) {
    switch(state) {
        case 0:
            switch(dir) {
                case 0:
                    return _standingNode_f;
                    break;
                case 1:
                    return _standingNode_fls;
                    break;
                case 2:
                    return _standingNode_l;
                    break;
                case 3:
                    return _standingNode_bls;
                    break;
                case 4:
                    return _standingNode_b;
                    break;
                case 5:
                    return _standingNode_brs;
                    break;
                case 6:
                    return _standingNode_r;
                    break;
                case 7:
                    return _standingNode_frs;
                    break;
                default:
                    CUAssertLog(false, "Invalid tile data.");
                    break;
            }
            break;
        default:
            CUAssertLog(false, "Invalid tile data.");
            break;
    }
}

/**
 * State:
 * 0: standing
 * 1: build up
 * 2: charging
 *
 * Direction:
 * 0->F / 1->FLS / 2->L / 3->BLS / 4->B
 * 5->BRS / 6->R / 7->FRS
 **/
std::shared_ptr<Node> PlayerModel::setTextNode(const std::shared_ptr<Node>& node, int state, int dir, bool set) {
    switch(state) {
        case 0:
            switch(dir) {
                case 0:
                    if(set) {
                        node->setVisible(false);
                        _standingNode_f = node;
                        _standingNode_f->setName("Standing Node f");
                        return NULL;
                    } else {
                        return _standingNode_f;
                    }
                    break;
                case 1:
                    if(set) {
                        node->setVisible(false);
                        _standingNode_fls = node;
                        _standingNode_fls->setName("Standing Node fls");
                        return NULL;

                    } else {
                        return _standingNode_fls;
                    }
                    break;
                case 2:
                    if(set) {
                    node->setVisible(false);
                    _standingNode_l = node;
                    _standingNode_l->setName("Standing Node l");
                        return NULL;

                    } else { return _standingNode_l;}
                    break;
                case 3:
                    if(set){
                    node->setVisible(false);
                    _standingNode_bls = node;
                    _standingNode_bls->setName("Standing Node bls");
                        return NULL;

                    }else{return _standingNode_bls;}
                    break;
                case 4:
                    if(set){
                    node->setVisible(false);
                    _standingNode_b = node;
                    _standingNode_b->setName("Standing Node b");
                        return NULL;

                    }else{return _standingNode_b;}
                    break;
                case 5:
                    if(set){
                        node->setVisible(false);
                    _standingNode_brs = node;
                    _standingNode_brs->setName("Standing Node brs");
                        return NULL;

                    }else{return _standingNode_brs;}
                    break;
                case 6:
                    if(set){
                        node->setVisible(false);
                    _standingNode_r = node;
                    _standingNode_r->setName("Standing Node r");
                        return NULL;
                    }else{return _standingNode_r;}
                    break;
                case 7:
                    if(set){
                        node->setVisible(false);
                    _standingNode_frs = node;
                    _standingNode_frs->setName("Standing Node frs");
                    return NULL;
                    }else{ return _standingNode_frs;}
                    break;
                default:
                    CUAssertLog(false, "Invalid tile data.");
                    break;
            }
            break;
        case 1:
        case 2:
            switch(dir) {
                case 0:
                    if(set) {
                        node->setVisible(false);
                        _chargingNode_f = node;
                        _chargingNode_f->setName("charging Node f");
                        return NULL;
                    } else {
                        return _chargingNode_f;
                    }
                    break;
                case 1:
                    if(set) {
                        node->setVisible(false);
                        _chargingNode_fls = node;
                        _chargingNode_fls->setName("charging Node fls");
                        return NULL;
                        
                    } else {
                        return _chargingNode_fls;
                    }
                    break;
                case 2:
                    if(set) {
                        node->setVisible(false);
                        _chargingNode_l = node;
                        _chargingNode_l->setName("charging Node l");
                        return NULL;
                        
                    } else { return _chargingNode_l;}
                    break;
                case 3:
                    if(set){
                        node->setVisible(false);
                        _chargingNode_bls = node;
                        _chargingNode_bls->setName("charging Node bls");
                        return NULL;
                        
                    }else{return _chargingNode_bls;}
                    break;
                case 4:
                    if(set){
                        node->setVisible(false);
                        _chargingNode_b = node;
                        _chargingNode_b->setName("charging Node b");
                        return NULL;
                        
                    }else{return _chargingNode_b;}
                    break;
                case 5:
                    if(set){
                        node->setVisible(false);
                        _chargingNode_brs = node;
                        _chargingNode_brs->setName("charging Node brs");
                        return NULL;
                        
                    }else{return _chargingNode_brs;}
                    break;
                case 6:
                    if(set){
                        node->setVisible(false);
                        _chargingNode_r = node;
                        _chargingNode_r->setName("charging Node r");
                        return NULL;
                    }else{return _chargingNode_r;}
                    break;
                case 7:
                    if(set){
                        node->setVisible(false);
                        _chargingNode_frs = node;
                        _chargingNode_frs->setName("charging Node frs");
                        return NULL;
                    }else{ return _chargingNode_frs;}
                    break;
                default:
                    CUAssertLog(false, "Invalid tile data.");
                    break;
            }
        default:
            CUAssertLog(false, "Invalid tile data.");
            break;
    }
}

/**
* Applies the impulse to the body of this player
*/
void PlayerModel::applyLinearImpulse(Vec2& impulse) {
    _body->ApplyLinearImpulseToCenter(IMPULSE_SCALE * b2Vec2(impulse.x,impulse.y), true);
}

/**
 * Returns true if the player is moving slow enough to sling
 */
bool PlayerModel::canSling(){
    return !_stunned && _body->GetLinearVelocity().Length() <= MAX_SPEED_FOR_SLING;
}

/**
* Returns true if player is in bounds
**/
bool PlayerModel::inBounds(int width, int height){
    b2Vec2 position = _body->GetPosition();
    return (position.x > 0 && (position.y-0.35) > 0 && position.x < width && (position.y-0.35) < height);
}

/**
* Updates the aim arrow.
*
* @param node  updates the aim arrow.
*/
void PlayerModel::updateArrow(cugl::Vec2 aim, bool visible) {
	aim *= -.3;
	cugl::Vec2 playerImageOffset = cugl::Vec2(_node->getWidth() / 2.0, _node->getHeight() / 2.0);

	float scaleFactor = aim.length();
	float angle = -1 * aim.getAngle();

	_arrow->setScale(cugl::Vec2(scaleFactor, 1));
	_arrow->setAngle(angle);
	_arrow->setPosition(cugl::Vec2(playerImageOffset.x + aim.x, playerImageOffset.y - aim.y));
	_arrow->setVisible(visible);
	_arrow->setColor(cugl::Color4::RED);
}

void PlayerModel::updateArrow(bool visible) {
	_arrow->setVisible(visible);
}


/**
* Updates the object's physics state (NOT GAME LOGIC). This is the method
* that updates the scene graph and is called after collision resolution.
*
* @param dt Timing values from parent loop
*/
void PlayerModel::update(float dt) {
	Obstacle::update(dt);
	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawscale);
		_node->setAngle(getAngle());
        if(_stunned){
            _node->setColor(Color4::GREEN);
        }
        else if(!canSling()){
            _node->setColor(Color4::RED);
        } else {
            _node->setColor(_color);
        }
	}
    if(_shouldStopSoon && Timestamp().ellapsedMillis(_collisionTimeout) >= COLLISION_TIMEOUT){
        _shouldStopSoon = false;
        _body->SetLinearVelocity(b2Vec2(0,0));
    }
    if(_stunOnStop && getLinearVelocity().length() <= MAX_SPEED_FOR_SLING){
        _stunOnStop = false;
        _stunned = true;
        _stunTimeout.mark();
    }
    if(_stunned && Timestamp().ellapsedMillis(_stunTimeout) >= _stunDuration){
        _stunned = false;
    }
}
