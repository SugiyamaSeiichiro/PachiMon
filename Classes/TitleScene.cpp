#include "TitleScene.h"
#include "SelectScene.h"
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
using namespace cocos2d;
// �^�C�g���V�[���쐬 ========================================
Scene* Title::createScene()
{
	auto scene = Scene::create();
	auto layer = Title::create();
	scene->addChild(layer);
	return scene;
}
// �������� ==================================================
bool Title::init()
{	
	if (!Layer::init())	
		return false;		
	// �w�i�̃X�v���C�g
	auto BackGround = Sprite::create("Back/TitleScene/BackGround.png");	
	BackGround->setPosition(Vec2(270.0f, 450.0f));	
	this->addChild(BackGround);	
	// �X�^�[�g�̃X�v���C�g
	auto Start = Sprite::create("UI/Start.png");
	Start->setPosition(Vec2(270.0f, 200.0f));
	this->addChild(Start);
	auto repeatForever = RepeatForever::create(Blink::create(1.2f, 1));
	Start->runAction(repeatForever);
	// �^�b�`�̂��߂̃��X�i�[���쐬����B
	auto listener = EventListenerTouchOneByOne::create();
	// ���X�i�[�ɃC�x���g�n���h���[��ݒ肷��B
	listener->onTouchBegan = CC_CALLBACK_2(Title::onTouchBegan, this);
	// �C�x���g�f�B�X�p�b�`���[���擾����B
	auto eventDispatcher = this->getEventDispatcher();
	// �C�x���g���X�i�[��ǉ�����B
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);	
	AudioEngine::play2d("Sound/BGM/TitleBGM.mp3",true,0.3f);	
	return true;
}
// �^�b�`�J�n�� ==============================================
bool Title::onTouchBegan(Touch *touch, Event *event)
{	
	AudioEngine::stopAll();
	AudioEngine::play2d("Sound/SE/Button.mp3",false,0.5f);	
	Director::getInstance()->replaceScene(
		TransitionFade::create(2.0f, Select::createScene()));	
	return true;
}