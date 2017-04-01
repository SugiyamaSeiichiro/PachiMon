#pragma once
#include "cocos2d.h"
#include "ui\CocosGUI.h"
class Select :public cocos2d::Layer
{
private:
	bool init()override;
	void selectedItemEvent(
		cocos2d::Ref *pSender, 
		cocos2d::ui::ListView::EventType type);
	void touchEvent(cocos2d::Ref *pSender,
		cocos2d::ui::Widget::TouchEventType type, int num);
	int BGM;		
	CREATE_FUNC(Select);
public:	
	static cocos2d::Scene* createScene();		
};