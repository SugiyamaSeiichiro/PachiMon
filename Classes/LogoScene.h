#pragma once
#include "cocos2d.h"
class Logo :public cocos2d::Layer
{
private:
	// ��������
	bool init()override;
	// �V�[���؂�ւ�
	void changeScene(float time);
	CREATE_FUNC(Logo);
public:
	// ���S�V�[���쐬
	static cocos2d::Scene* createScene();	
};