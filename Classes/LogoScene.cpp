#include "LogoScene.h"
#include "TitleScene.h"
using namespace cocos2d;
// ロゴシーン作成
Scene* Logo::createScene()
{
	auto scene = Scene::create();
	auto layer = Logo::create();
	scene->addChild(layer);
	return scene;
}
// 初期処理
bool Logo::init()
{
	if (!Layer::init())	
		return false;		
	// ロゴ
	Sprite* Logo = Sprite::create("UI/Logo.png");	
	Logo->setPosition(Vec2(270.0f, 450.0f));	
	this->addChild(Logo);	
	this->scheduleOnce(
		schedule_selector(Logo::changeScene), 0.5f);	
	return true;
}
// シーン切り替え
void Logo::changeScene(float time)
{		
	TransitionFade* fade = TransitionFade::create(
		2.0f, Title::createScene());
	Director::getInstance()->replaceScene(fade);
}