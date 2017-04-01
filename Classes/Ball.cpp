#include "Ball.h"
USING_NS_CC;
void Ball::Collision(cocos2d::Node* node, float radius1, float radius2){
	Vec2 pos1 = this->getPosition();
	Vec2 pos2 = node->getPosition();	
	if (((pos2.x - pos1.x)*(pos2.x - pos1.x) +
		(pos2.y - pos1.y)*(pos2.y - pos1.y)) <=
		((radius1 + radius2)*(radius1 + radius2))){		
		Vec2 n = (pos2 - pos1);
		n.normalize();
		float len = this->spd.dot(n);
		Vec2 spd_a = n*len;
		Vec2 spd_b = this->spd - spd_a;
		this->spd = -spd_a + spd_b;		
	}
}