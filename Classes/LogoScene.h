#pragma once
#include "cocos2d.h"
class Logo :public cocos2d::Layer
{
private:
	// 初期処理
	bool init()override;
	// シーン切り替え
	void changeScene(float time);
	CREATE_FUNC(Logo);
public:
	// ロゴシーン作成
	static cocos2d::Scene* createScene();	
};