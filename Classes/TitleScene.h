#pragma once
#include "cocos2d.h"
class Title :public cocos2d::Layer
{
private:
	// 初期処理
	bool init()override;
	// タッチ開始時
	bool onTouchBegan(
		cocos2d::Touch* touch,
		cocos2d::Event* event);	
	CREATE_FUNC(Title);
public:
	// タイトルシーン作成
	static cocos2d::Scene* createScene();	
};