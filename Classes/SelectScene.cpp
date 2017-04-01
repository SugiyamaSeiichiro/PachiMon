#include "SelectScene.h"
#include "PlayScene.h"
#include "audio/include/AudioEngine.h"
#include "Global.h"
#pragma execution_character_set("utf-8")
using namespace cocos2d;
// 概要：シーン生成
Scene* Select::createScene()
{
	auto scene = Scene::create();
	auto layer = Select::create();
	scene->addChild(layer);
	return scene;
}
// 概要：初期化
bool Select::init()
{
	if (!Layer::init())
		return false;
	// 背景
	Sprite* BackGround = Sprite::create("Back/SelectScene/BackGround.png");
	BackGround->setPosition(Vec2(270.0f, 450.0f));
	this->addChild(BackGround);
	// リストビュー作成
	ui::ListView* listView = ui::ListView::create();
	listView->setContentSize(Size(300, 400));
	listView->setPosition(Vec2(270.0f - listView->getContentSize().width / 2,
		550.0f - listView->getContentSize().height/2));
	listView->setDirection(ui::ScrollView::Direction::VERTICAL);
	listView->setBounceEnabled(true);
	listView->setItemsMargin(20.0f);
	this->addChild(listView);
	for (int i = 1; i <= 3; i++)
	{		
		auto button = ui::Button::create("UI/List.png");		
		button->setScale9Enabled(true);
		button->setContentSize(Size(240, 60));
		button->setPosition(button->getContentSize() / 2+Size(20,0));
		button->setTitleText(StringUtils::format("ダンジョン %d", i));
		button->setTitleColor(Color3B::RED);
		button->setTitleFontSize(30.0f);
		auto layout = ui::Layout::create();
		layout->setContentSize(button->getContentSize());
		layout->addChild(button);		
		listView->addChild(layout);	
	}	
	listView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(Select::selectedItemEvent, this));
	ui::PageView* pageView = ui::PageView::create();
	pageView->setDirection(ui::PageView::Direction::HORIZONTAL);
	pageView->setContentSize(Size(280, 80));
	pageView->setPosition(Vec2(50.0f,50.0f));		
	for (int i = 0; i < 9; i++){	
		auto layout = ui::Layout::create();
		layout->setContentSize(Size(240, 60));
		ui::ImageView* imageView = ui::ImageView::create("cocosui/scrollviewbg.png");
		imageView->setScale9Enabled(true);
		imageView->setContentSize(Size(240, 60));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
		layout->addChild(imageView);	
		auto button = ui::Button::create("UI/List.png");		
		button->setContentSize(Size(240, 60));
		button->setPosition(layout->getContentSize() / 2 + Size(10, 0));
		button->setTitleText(StringUtils::format("BGM %d", i+1));
		button->setTitleColor(Color3B::RED);
		button->setTitleFontSize(30.0f);
		button->addTouchEventListener(CC_CALLBACK_2(Select::touchEvent, this,i));	
		layout->addChild(button);		
		pageView->insertPage(layout, i);		
	}
	this->addChild(pageView);
	g_data.selectBGM = 0;
	return true;
}
void Select::selectedItemEvent(
	Ref *pSender, ui::ListView::EventType type)
{	
	if(type == cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END)	
	{
		auto listView = static_cast<ui::ListView*>(pSender);
		g_data.stage = listView->getCurSelectedIndex();
		experimental::AudioEngine::stopAll();
		experimental::AudioEngine::play2d("Sound/SE/Button.mp3", false, 0.5f);
		Director::getInstance()->replaceScene(Play::createScene());
	}
}
void Select::touchEvent(Ref *pSender,
	ui::Widget::TouchEventType type, int num)
{
	if(type == ui::Widget::TouchEventType::BEGAN)	
	{
		static int oldNum = 1;
		if (num != oldNum)
		{
			experimental::AudioEngine::stop(BGM);
			g_data.selectBGM = num;
			char s[128];
			sprintf(s, "sound/BGM/BGM(%d).mp3", g_data.selectBGM);
			BGM = experimental::AudioEngine::play2d(s);
			experimental::AudioEngine::setLoop(BGM, true);
			experimental::AudioEngine::setVolume(BGM, 0.4f);
			oldNum = g_data.selectBGM;
		}		
	}	
}
