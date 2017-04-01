#include "LogoScene.h"
#include "TitleScene.h"
using namespace cocos2d;
// ���S�V�[���쐬
Scene* Logo::createScene()
{
	auto scene = Scene::create();
	auto layer = Logo::create();
	scene->addChild(layer);
	return scene;
}
// ��������
bool Logo::init()
{
	if (!Layer::init())	
		return false;		
	// ���S
	Sprite* Logo = Sprite::create("UI/Logo.png");	
	Logo->setPosition(Vec2(270.0f, 450.0f));	
	this->addChild(Logo);	
	this->scheduleOnce(
		schedule_selector(Logo::changeScene), 0.5f);	
	return true;
}
// �V�[���؂�ւ�
void Logo::changeScene(float time)
{		
	TransitionFade* fade = TransitionFade::create(
		2.0f, Title::createScene());
	Director::getInstance()->replaceScene(fade);
}