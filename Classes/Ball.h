#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__
#include "cocos2d.h"
class Ball :public cocos2d::Layer{
private:
	cocos2d::Vec2 spd;
public:
	// XVˆ—
	void update(float delta);
	void Collision(cocos2d::Node* node, float radius1, float radius2);
	CREATE_FUNC(Ball);
};
#endif