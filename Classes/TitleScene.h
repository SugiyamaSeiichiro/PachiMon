#pragma once
#include "cocos2d.h"
class Title :public cocos2d::Layer
{
private:
	// ��������
	bool init()override;
	// �^�b�`�J�n��
	bool onTouchBegan(
		cocos2d::Touch* touch,
		cocos2d::Event* event);	
	CREATE_FUNC(Title);
public:
	// �^�C�g���V�[���쐬
	static cocos2d::Scene* createScene();	
};