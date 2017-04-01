#include "TitleScene.h"
#include "SelectScene.h"
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
using namespace cocos2d;
// タイトルシーン作成 ========================================
Scene* Title::createScene()
{
	auto scene = Scene::create();
	auto layer = Title::create();
	scene->addChild(layer);
	return scene;
}
// 初期処理 ==================================================
bool Title::init()
{	
	if (!Layer::init())	
		return false;		
	// 背景のスプライト
	auto BackGround = Sprite::create("Back/TitleScene/BackGround.png");	
	BackGround->setPosition(Vec2(270.0f, 450.0f));	
	this->addChild(BackGround);	
	// スタートのスプライト
	auto Start = Sprite::create("UI/Start.png");
	Start->setPosition(Vec2(270.0f, 200.0f));
	this->addChild(Start);
	auto repeatForever = RepeatForever::create(Blink::create(1.2f, 1));
	Start->runAction(repeatForever);
	// タッチのためのリスナーを作成する。
	auto listener = EventListenerTouchOneByOne::create();
	// リスナーにイベントハンドラーを設定する。
	listener->onTouchBegan = CC_CALLBACK_2(Title::onTouchBegan, this);
	// イベントディスパッチャーを取得する。
	auto eventDispatcher = this->getEventDispatcher();
	// イベントリスナーを追加する。
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);	
	AudioEngine::play2d("Sound/BGM/TitleBGM.mp3",true,0.3f);	
	return true;
}
// タッチ開始時 ==============================================
bool Title::onTouchBegan(Touch *touch, Event *event)
{	
	AudioEngine::stopAll();
	AudioEngine::play2d("Sound/SE/Button.mp3",false,0.5f);	
	Director::getInstance()->replaceScene(
		TransitionFade::create(2.0f, Select::createScene()));	
	return true;
}