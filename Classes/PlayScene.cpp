#include "PlayScene.h"
#include "SelectScene.h"
#include "audio/include/AudioEngine.h"
#include "Global.h"
#pragma execution_character_set("utf-8")
using namespace cocos2d::experimental;
USING_NS_CC;
// �v���C�V�[���쐬 =========================================
Scene* Play::createScene()
{	
	auto scene = Scene::create();
	auto layer = Play::create();
	scene->addChild(layer);	
	return scene;
}
// �������� =================================================
bool Play::init(){
	if (!Layer::init())
		return false;	
	BGMCreate();
	//�U���p�[�e�B�N��	
	Fire = ParticleSun::create();	
	Fire->setPosition(Vec2(270.0f, 0.0f));
	Fire->setVisible(false);
	Fire->setScale(2.0f);
	this->addChild(Fire,2);	
	LabelCreate();
	SpriteCreate();
	// �^�b�`�̂��߂̃��X�i�[���쐬����B
	auto listener = EventListenerTouchOneByOne::create();
	// ���X�i�[�ɃC�x���g�n���h���[��ݒ肷��B
	listener->onTouchBegan = CC_CALLBACK_2(Play::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Play::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Play::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(Play::onTouchCancelled, this);
	// �C�x���g�f�B�X�p�b�`���[���擾����B
	auto eventDispatcher = this->getEventDispatcher();
	// �C�x���g���X�i�[��ǉ�����B
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	MapChipCreate();
	initializ();	
	return true;	
}
// ������ ===================================================
void Play::initializ(){			
	for (int i = 0; i < BALLNUM; i++)
	{		
		ReBall[i].type = Probability();
		ReBall[i].sprite->setVisible(true);
		ReBall[i].sprite->setPosition(Vec2(400.0f + (i*22.0f), 850.0f - (i*5.5f)));
	}
	GameOver->setVisible(false);
	for (int i = 0; i < 2; i++)
	{
		Button[i]->setVisible(false);
		Gate[i].type = 0;
	}
	Ball.sprite->setVisible(false);
	Ball.cnt = BALLNUM;	
	Attack.type = 0;
	Cannon.type = 0;
	switch (g_data.stage)	
	{
	case 0:	HP = 2000;break;
	case 1:	HP = 4000; break;
	case 2:HP = 999999;break;
	}	
	Playing = false;
	this->scheduleUpdate();
}
// �X�V���� =================================================
void Play::update(float delta){			
	AudioEngine::setVolume(BGM, Volume[BGMVolume]);
	HoldColor(Ball.type,Ball.sprite);
	for (int i = 0; i < BALLNUM; i++)
		HoldColor(ReBall[i].type, ReBall[i].sprite);
	EffectUpdate();
	EnemyAnimation();
	//�|�[�Y�ȊO�Ȃ�
	if (!(Director::getInstance()->isPaused()))
	{
		//���x���X�V
		LabelUpdate();
		//�X�v���C�g�̈ʒu���擾
		Ball.pos = Ball.sprite->getPosition();		
		//�{�[�����ˌ�
		if (Ball.sprite->isVisible() && Playing == true) 
		{				
			//�{�[�����O�ɏo����d�͂�������
			if (CollisionCnt <= 0 && Ball.sprite->isVisible())
			{
				Ball.spd.y += 9.8f/32;
				switch (g_data.stage){
				case 0:
					if (Ball.pos.x + 10 <= 270)
						Character->setTexture("Chara/Player2.png");
					else Character->setTexture("Chara/Player.png");
					break;
				case 1:
					if (Ball.pos.x + 10 <= 270)
						Character->setTexture("Chara/Player2.png");
					else Character->setTexture("Chara/Player.png");
					break;			
				}
			}
			//�I�u�W�F�N�g�̓����蔻��
			ObjectCollision();						
			//���W�X�V
			Ball.pos -= Ball.spd;				
			//�J�E���^�[			
			CollisionCnt--;			
		}
		ChuckerUpdate();
		if(g_data.stage!=2)
		GateUpdate();
		AttackUpdate();		
		GameClearUpdate();
		GameOverUpdate();		
		Ball.sprite->setPosition(Ball.pos);		
		BallStreak->setPosition(Ball.pos);		
		String *DamageCnt = String::createWithFormat("HP %d ", HP);
		label[HPLabel]->setString(DamageCnt->getCString());		
		TouchCnt++;
		//�����������甭�˂��Ȃ�
		if (TouchCnt >= 20)
			TouchFlag = false;			
		String *Update = String::createWithFormat("pos.x=%.2f\npos.y=%.2f\nspd.x=%.2f\nspd.y=%.2f\nangle=%f",
			Ball.pos.x, Ball.pos.y, Ball.spd.x, Ball.spd.y,Cannon.angle);
		label[UpdateLabel]->setString(Update->getCString());
		if (Playing == false &&Attack.type == 0 &&
			GameOver->isVisible() == false && GameClear[0]->isVisible() == false)
		{
			if (Meter <= 200)
				Meter += 4.0f;
			else Meter = 4.0f;
			Power->setTextureRect(Rect(0.0f, 0.0f, Meter, 20.0f));
		}		
	}	
	if (Playing == false&&Attack.type==0)
		Comb = 0;
	SoundTime--;
}
// �^�b�`�J�n���� ===========================================
bool Play::onTouchBegan(Touch *touch, Event *event)
{	
	auto touchPosition = touch->getLocation();	
	TouchFlag = true;		
	TouchCnt = 0;
	//�t�F�[�h���ԁA�\�����n�܂鋗���A�摜�T�C�Y
	Streak = MotionStreak::create(1.0f, 1.0f, 20.0f, Color3B(0xff, 0xff, 0xff), "UI/Effect.png");
	Streak->setPosition(touchPosition);		
	this->addChild(Streak, 6);
	//�|�[�Y	
	Rect ButtonRect;
	ButtonRect = Rect(Button[2]->getPosition().x - Button[2]->getContentSize().width / 2,
		Button[2]->getPosition().y - Button[2]->getContentSize().height / 2,
		Button[2]->getContentSize().width,
		Button[2]->getContentSize().height);
	if (ButtonRect.containsPoint(touchPosition) && GameOver->isVisible() == false)
	{
		int id = AudioEngine::play2d("Sound/SE/Button.mp3");
		AudioEngine::setVolume(id, Volume[ObjectVolume]);
		TouchFlag = false;		
		if (Director::getInstance()->isPaused()){
			Director::getInstance()->resume();
			label[PauseLabel]->setVisible(false);
		}
		else {
			Director::getInstance()->pause();
			label[PauseLabel]->setVisible(true);
		}		
	}	
	Rect CharaRect;
	CharaRect = Rect(Character->getPosition().x - Character->getContentSize().width / 2,
		Character->getPosition().y - Character->getContentSize().height / 2,
		Character->getContentSize().width,
		Character->getContentSize().height);
	if (CharaRect.containsPoint(touchPosition) && GameOver->isVisible() == false)
	{		
		if (SoundTime <= 0)
		{			
			int sound = rand() % 2;
			switch (g_data.stage)
			{
			case 0:
				switch (sound)
				{
				case 0:
				{
						   int id = AudioEngine::play2d("Sound/VOICE/TwentyPercent.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);
						   break;
				}
				case 1:
				{
						   int id = AudioEngine::play2d("Sound/VOICE/TUOW.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);						 
						   break;
				}
				}
				break;
			case 1:
				switch (sound){
				case 0:{
						   int id = AudioEngine::play2d("Sound/VOICE/Six.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);
						   break;
				}
				case 1:
				{
						   int id = AudioEngine::play2d("Sound/VOICE/Money.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);
						   break;
				}
				}
				break;
			case 2:
				switch (sound)
				{
				case 0:
				{
						   int id = AudioEngine::play2d("Sound/VOICE/W.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);
						   break;
				}
				case 1:
				{
						   int id = AudioEngine::play2d("Sound/VOICE/W2.mp3");
						   AudioEngine::setVolume(id, Volume[ObjectVolume]);
						   break;
				}
				}
				break;
			}
			SoundTime = 160;
		}
		TouchFlag = false;		
	}	
	String *Touch = String::createWithFormat("�^�b�`�J�n");
	label[TouchLabel]->setString(Touch->getCString());	
	return true;
}
// �^�b�`�ړ����� ===========================================
void Play::onTouchMoved(Touch *touch, Event *event){	
	//���݃^�b�`�ʒu
	auto touchPosition = touch->getLocation();
	Streak->setPosition(touchPosition);
	//�ړ��݂̂Ŕ��˂��Ȃ��悤�ɂ���
	TouchFlag = false;	
	String *Touch = String::createWithFormat("�^�b�`�ړ�");
	label[TouchLabel]->setString(Touch->getCString());
	if (GameOver->isVisible() == false && Director::getInstance()->isPaused() == false){
		//�P�t���[���O�̃^�b�`�ʒu
		auto touchPositionBefore = touch->getPreviousLocation();			
		//�^�b�`�ړ��Ŏ�C�p�x�ړ�			
		Vec2 a = touchPositionBefore - Vec2(270.0f, 750.0f);
		Vec2 b = touchPosition - Vec2(270.0f, 750.0f);
		float angle = atan2f(a.y, a.x);
		float angle2 = atan2f(b.y, b.x);
		angle = angle2 - angle;
		angle = CC_RADIANS_TO_DEGREES(angle);		
		if (Cannon.angle<=70 )
		{
			Cannon.angle -= angle;
			Cannon.sprite->setRotation(Cannon.angle);
		}		
		else if (Cannon.angle >= -70)
		{
			Cannon.angle -= angle;
			Cannon.sprite->setRotation(Cannon.angle);
		}
		if (Cannon.angle >= 70)
			Cannon.angle=69;
		else if(Cannon.angle <= -70)
			Cannon.angle=-69;	
		if (Playing == false)
		{
			switch (g_data.stage)
			{
			case 0:
				if (Cannon.angle > 0)
					Character->setTexture("Chara/Player2.png");
				else Character->setTexture("Chara/Player.png");
				break;
			case 1:
				if (Cannon.angle > 0)
					Character->setTexture("Chara/Player2.png");
				else Character->setTexture("Chara/Player.png");
				break;
			}
		}
	}
}
// �^�b�`�I������ ===========================================
void Play::onTouchEnded(Touch *touch, Event *event)
{	
	auto touchPosition = touch->getLocation();		
	String *Touch = String::createWithFormat("�^�b�`�I��");
	label[TouchLabel]->setString(Touch->getCString());	
	//�ړ������Ƀ^�b�`����������
	if (TouchFlag == true && Ball.cnt != 0 && Director::getInstance()->isPaused() == false &&
		Attack.type==0 && Playing==false &&GameClear[0]->isVisible()==false) 
	{
		if (Ball.sprite->isVisible() == false)
		{
			Ball.sprite->setVisible(true);	
			Comb = 0;
			Playing = true;					
			//Cannon.angle-=0.001f;	
			Cannon.cnt = 0;
			Cannon.type = 0;			
			Effect->setRotation(Cannon.angle);			
			float angle = CC_DEGREES_TO_RADIANS(Cannon.angle);			
			float speed = 0.2f * (Meter / 4.0f) + 4.0f;			
			CollisionCnt = 110 / speed;		
			if (g_data.stage == 2)
				speed = 8;
			Ball.spd = Vec2(static_cast<float>(sin(angle) * speed),
				static_cast<float>(cos(angle) * speed));							
			Ball.type = ReBall[0].type;			
			for (int i = 0; i < Ball.cnt; i++)
				ReBall[i].type = ReBall[i + 1].type;				
			Ball.cnt--;
			ReBall[Ball.cnt].sprite->setVisible(false);				
			int id = AudioEngine::play2d("Sound/SE/Cannon.mp3");		
			label[CombLabel]->setColor(Color3B::YELLOW);
			BallStreak->setVisible(true);
			cocos2d::Rect bb = Cannon.sprite->getBoundingBox();
		}		
	}
	//�Q�[���I�[�o�[�ɂȂ�����
	if (TouchFlag == true && GameOver->isVisible() == true)
	{		
		Rect ButtonRect[2];
		for (int i = 0; i < 2; i++)
		{
			ButtonRect[i] = Rect(Button[i]->getPosition().x - Button[i]->getContentSize().width / 2,
				Button[i]->getPosition().y - Button[i]->getContentSize().height / 2,
				Button[i]->getContentSize().width,
				Button[i]->getContentSize().height);
		}
		if (ButtonRect[0].containsPoint(touchPosition))
		{
			initializ();
			AudioEngine::resume(BGM);
			int id = AudioEngine::play2d("Sound/SE/Button.mp3");
			AudioEngine::setVolume(id, Volume[ObjectVolume]);
		}
		if (ButtonRect[1].containsPoint(touchPosition))
		{
			Director::getInstance()->replaceScene(Select::createScene());
			BGM = 0;
			int id = AudioEngine::play2d("Sound/SE/Button.mp3");
			AudioEngine::setVolume(id, Volume[ObjectVolume]);
		}
	}
	if (TouchFlag == true && GameClear[1]->isVisible() == true)
	{
		Rect ButtonRect;		
		ButtonRect = Rect(Button[0]->getPosition().x - Button[0]->getContentSize().width / 2,
			Button[0]->getPosition().y - Button[0]->getContentSize().height / 2,
			Button[0]->getContentSize().width,
			Button[0]->getContentSize().height);
		if (ButtonRect.containsPoint(touchPosition))
		{			
			int id = AudioEngine::play2d("Sound/SE/Button.mp3");
			AudioEngine::setVolume(id, Volume[ObjectVolume]);
			AudioEngine::stop(BGM);
			BGM = 0;
			Director::getInstance()->replaceScene(Select::createScene());
		}		
	}
}
// �ۗ��F�m�� ===============================================
int Play::Probability()
{	
	int color;
	int probability = rand() % 30;
	switch (probability)
	{
	case 0: color = 4; break;
	case 1: case 2: color = 3; break;
	case 3: case 4: case 5: color = 2; break;
	case 6: case 7: case 8: case 9: color = 1; break;
	default: color = 0; break;
	}
	return color;
}
// �ۗ��F ===================================================
void Play::HoldColor(int type,Sprite* sprite)
{			
	switch (type)
	{
	case 0:	sprite->setColor(Color3B::WHITE); break;
	case 1:	sprite->setColor(Color3B(100, 180, 250)); break;
	case 2: sprite->setColor(Color3B::GREEN); break;
	case 3: sprite->setColor(Color3B::RED); break;
	case 4: sprite->setColor(Color3B::YELLOW); break;
	}	
}
// �l�p���m�̓����蔻�� =====================================
bool Play::CollisionS(Vec2 square, Vec2 square2,Vec2 radius2)
{
	if (square.x + 10.0f >= square2.x - radius2.x &&
		square.x - 10.0f <= square2.x + radius2.x &&
		square.y + 10.0f >= square2.y - radius2.y &&
		square.y - 10.0f <= square2.y + radius2.y)
		return true;
	else return false;
}
// �~���m�̓����蔻�� =======================================
bool Play::CollisionC(Vec2 circle, Vec2 circle2,float radius2)
{
	if (((circle2.x - circle.x)*(circle2.x - circle.x) +
		(circle2.y - circle.y)*(circle2.y - circle.y)) <=
		((10.0f + radius2)*(10.0f + radius2)))
		return true;
	else return false;
}
// �����I���� ===============================================
Vec2 Play::Reflection(object fall,Vec2 obstacle)
{
	Vec2 n = (obstacle - fall.pos);
	n.normalize();	
	float len = fall.spd.dot(n);
	Vec2 spd_a = n*len;
	Vec2 spd_b = fall.spd - spd_a;
	//fall.spd = -spd_a + spd_b;	
	return -spd_a + spd_b;
}
// �Q�[�g�X�V���� ===========================================
void Play::GateUpdate(){
	for (int i = 0; i < 2; i++)
	{
		Gate[i].cnt++;
		if (Gate[i].type == 1)
		{
			if (Gate[i].cnt >= 160 && Gate[i].sprite->isVisible())
			{
				if (i == 0){
					Ball.pos = Gate[1].pos;
					Gate[i].sprite->setVisible(false);
					ActionInterval* action;
					action = ScaleTo::create(0.2f, 0.0f);
					Gate[1].sprite->runAction(action);
				}
				else 
				{
					Ball.pos = Gate[0].pos;
					Gate[i].sprite->setVisible(false);
					ActionInterval* action;
					action = ScaleTo::create(0.2f, 0.0f);
					Gate[0].sprite->runAction(action);
				}
				Gate[i].type = 2;
				CollisionCnt = 5;
				Ball.sprite->setVisible(true);
				BallStreak = MotionStreak::create(
					0.3f, 1.0f, 25.0f, Color3B::WHITE, "Effect/Effect(3).png");
				this->addChild(BallStreak, 2);
			}
		}
		if (Gate[i].type == 2 && Gate[i].cnt >= 165)
		{
			if (i == 0)
				Gate[1].sprite->setVisible(false);
			else Gate[0].sprite->setVisible(false);
			Gate[i].type = 3;
		}
		if (Gate[i].type == 3 && Gate[i].cnt >= 200)
		{
			GatePosition();
			for (int j = 0; j < 2; j++)
			{
				Gate[j].sprite->setVisible(true);
				Gate[j].sprite->setScale(1.0f);
			}
			Gate[i].type = 0;
		}
	}
}
// �U���X�V���� =============================================
void Play::AttackUpdate()
{
	if (Special->isVisible() && Attack.cnt <= 60)
	{
		Attack.cnt++;
		if (Attack.cnt >= 60)
		{
			Special->setVisible(false);
			label[AttackLabel]->setVisible(true);				
			Fire->setPosition(Vec2(270.0f, 0.0f));
			Fire->setVisible(true);
		}
	}
	if (label[AttackLabel]->isVisible())
	{		
		Attack.pos = Fire->getPosition();
		if (CollisionS(Vec2(270.0f, 290.0f),
			Attack.pos - Vec2(0.0f, 250.0f), Vec2(10.0f, 250.0f)) == true)
		{
			switch (g_data.stage)
			{
			case 0:particle->setStartColor(Color4F::YELLOW);break;
			case 1:							
				particle->setStartColor(Color4F::RED);
				particle->setEndColor(Color4F::BLUE);				
				break;
			case 2:				
				particle->setStartColor(Color4F::WHITE);
				particle->setEndColor(Color4F::RED);
				break;
			}			
			ParticleCreate(Vec2(0.0f, -400.0f), 500.0f, Vec2(270.0f, 295.0f));
			int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
			AudioEngine::setVolume(id, Volume[BallVolume]);
			if (HP>0)
			HP -= Comb*2*(Ball.type+1)*(Meter/250+0.8f);
			else HP = 0;
		}
		Attack.pos.y += 10.5f;		
		Fire->setPosition(Attack.pos);
		if (Attack.pos.y >= 1500.0f)
		{			
			Fire->setVisible(false);
			label[AttackLabel]->setVisible(false);
			Attack.type = 0;
		}
	}	
	if (HP <= 0)
		Enemy.sprite->setVisible(false);
}
// �I�u�W�F�N�g�X�V���� =====================================
void Play::ObjectCollision(){
	//�ʒu�擾
	for (int i = 0; i < OBJECTNUM; i++){
		Peg[i].pos = Peg[i].sprite->getPosition();
		Box[i].pos = Box[i].sprite->getPosition();		
	}
	for (int i = 0; i < 2;i++)
	Gate[i].pos = Gate[i].sprite->getPosition();
	//�{�[���Ƒ�C�{�̂̓����蔻��
	if (CollisionC(Ball.pos, Vec2(270.0f, 840.0f), 60.0f)&&CollisionCnt <= 0)
	{
		Ball.spd = Reflection(Ball, Vec2(270.0f, 840.0f));
		CollisionCnt = 2;
	}
	if (g_data.stage!=2)
	{
		if (CollisionS(Ball.pos, Vec2(270.0f, 300.0f),
			Vec2(50.0f, 40.0f)) && CollisionCnt <= 0)
		{
			int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
			AudioEngine::setVolume(id, Volume[BallVolume]);
			Ball.spd = Reflection(Ball, Vec2(270.0f, 300.0f));
			CollisionCnt = 2;
			Comb++;
		}
	}
	if (CollisionS(Ball.pos, Vec2(200.0f, 130.0f), Vec2(20.0f, 30.0f))&&Ball.spd.x<0||
		(CollisionS(Ball.pos, Vec2(340.0f, 130.0f), Vec2(20.0f, 30.0f)) && Ball.spd.x>0))
	{
		int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
		AudioEngine::setVolume(id, Volume[BallVolume]);
		Ball.spd = Vec2(-Ball.spd.x, Ball.spd.y);
		Comb++;
	}	
	//�{�[���ƓB�̓����蔻��
	for (int i = 0; i < OBJECTNUM; i++)
	{
		if (CollisionC(Ball.pos, Peg[i].pos, 10.0f)&& CollisionCnt <= 0)
		{
			if (g_data.stage != 2)
			for (int j = 0; j < 10; j++)
				ParticleCreate(Vec2(0.0f, -300.0f), 400.0f, Peg[i].pos);			
			int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
			AudioEngine::setVolume(id, Volume[BallVolume]);
			Ball.spd = Reflection(Ball, Peg[i].pos);
			CollisionCnt = 2;			
			Comb++;			
		}
		//�{�[���ƐԔ��̓����蔻��		
		if (CollisionC(Ball.pos, Box[i].pos, 10.0f)&& CollisionCnt <= 0)
		{
			int id = AudioEngine::play2d("Sound/SE/Box.mp3");
			Ball.pos.y = Box[i].pos.y + 20.0f;
			if (Ball.pos.x < 270.0f)
				Ball.spd.x = -1;
			else Ball.spd.x = 1;
			Ball.spd.y = -1;			
			CollisionCnt = 1;			
		}		
	}		
	//�{�[���ƃQ�[�g�̓����蔻��
	if (g_data.stage != 2) 
	{
		for (int i = 0; i < 2; i++) 
		{
			if (CollisionC(Ball.pos, Gate[i].pos, 18.0f) &&
				Ball.sprite->isVisible() && Gate[i].sprite->isVisible() && CollisionCnt <= 0) {
				int id = AudioEngine::play2d("Sound/SE/Gate.mp3");
				AudioEngine::setVolume(id, Volume[GateVolume]);
				Ball.sprite->setVisible(false);
				Gate[i].cnt = 0;
				Gate[i].type = 1;
				ActionInterval* action;
				action = ScaleTo::create(2, 0.0f);
				Gate[i].sprite->runAction(action);
			}
		}
	}
	//�{�[���ƃA�^�b�N�`���b�J�[�̓����蔻��	
	if (CollisionS(Ball.pos, Vec2(270.0f,115.0f),Vec2(30.0f, 2.0f))&&Ball.spd.y>0)
	{	
		for (int j = 0; j < 10; j++)
		{
			ParticleCreate(Vec2(0.0f, -300.0f), 250.0f, Vec2(270.0f, 130.0f));
			particle->setStartColor(Color4F::ORANGE);
		}
		int id = AudioEngine::play2d("Sound/VOICE/Special.mp3");
		AudioEngine::setVolume(id, Volume[ObjectVolume]);
		String *AttackCnt = String::createWithFormat("%d Attack �~ %d HoldColor �~ %0.2f", Comb * 100, Ball.type + 1, Meter / 250.0f + 0.8f);
		label[AttackLabel]->setString(AttackCnt->getCString());
		Special->setVisible(true);
		Ball.sprite->setVisible(false);
		Ball.pos = Vec2(270.0f, 840.0f);
		Ball.spd = Vec2(0.0f, 0.0f);
		Attack.cnt = 0;
		Attack.type = 1;
		Playing = false;	
		BallStreak->setVisible(false);
	}
	//�ǂ̔���
	if ((Ball.pos.x + 10.0f >= 540.0f&&Ball.spd.x<0)||
		(Ball.pos.x - 10.0f <= 0.0f&&Ball.spd.x>0))
		Ball.spd = Vec2(-Ball.spd.x, Ball.spd.y);	
	//�V��(��)
	if (Ball.pos.y + 10.0f >= 840.0f&& Ball.spd.y<0)
		Ball.spd = Vec2(Ball.spd.x, -Ball.spd.y);		
	//��ʉ��ɂ������甭�ˑO���
	if (Ball.pos.y - 10.0f <= 80.0f)
	{
		int id = AudioEngine::play2d("Sound/SE/Fall.mp3");
		AudioEngine::setVolume(id, Volume[ObjectVolume]);
		for (int i = 0; i < 10; i++)
		{
			ParticleCreate(Vec2(0.0f, 300.0f), 
				400.0f, Vec2(Ball.pos.x, 100.0f));
			particle->setStartColor(Color4F::RED);
		}
		Ball.sprite->setVisible(false);
		Ball.pos = Vec2(270.0f, 840.0f);
		Ball.spd = Vec2(0.0f, 0.0f);
		Playing = false;		
		BallStreak->setVisible(false);
	}
}
// �`���b�J�[�X�V���� =======================================
void Play::ChuckerUpdate()
{
	for (int i = 0; i < 2; i++)
	{
		//�`���b�J�[�X�V����	
		Chucker[i].pos = Chucker[i].sprite->getPosition();
		if (Chucker[i].pos.x + 30 >= 180.0f + (i*360.0f))
			Chucker[i].spd.x *= -1;
		if (Chucker[i].pos.x - 30 <= 0.0f + (360.0f*i))
			Chucker[i].spd.x *= -1;
		Chucker[i].pos += Chucker[i].spd;
		Chucker[i].sprite->setPosition(Chucker[i].pos);
		//�{�[���ƃ`���b�J�[�̍��[�̓����蔻��
		if (CollisionS(Ball.pos, Chucker[i].pos - Vec2(27.0f, 0.0f),
			Vec2(3.0f, 15.0f)) && CollisionCnt <= 0)
		{
			int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
			AudioEngine::setVolume(id, Volume[BallVolume]);
			Ball.spd = Reflection(Ball, Chucker[i].pos);
			CollisionCnt = 2;			
			Comb++;
		}
		//�{�[���ƃ`���b�J�[�̉E�[�̓����蔻��
		if (CollisionS(Ball.pos, Chucker[i].pos + Vec2(27.0f, 0.0f),
			Vec2(3.0f, 15.0f)) && CollisionCnt <= 0){
			int id = AudioEngine::play2d("Sound/SE/Ball.mp3");
			AudioEngine::setVolume(id, Volume[BallVolume]);
			Ball.spd = Reflection(Ball, Chucker[i].pos);
			CollisionCnt = 2;			
			Comb++;
		}
		//�{�[���ƃ`���b�J�[�̌��̓����蔻��
		if (CollisionS(Ball.pos, Chucker[i].pos,Vec2(23.0f, 2.0f)))
		{
			for (int j = 0; j < 10; j++)
			{
				ParticleCreate(Vec2(0.0f, 400.0f), 250.0f,
					Vec2(Chucker[i].pos.x+Chucker[i].spd.x*20,120.0f));
				particle->setStartColor(Color4F::GREEN);
				particle->setDuration(0.2f);
			}
			int id = AudioEngine::play2d("Sound/SE/FreeBall.mp3");
			AudioEngine::setVolume(id, Volume[ObjectVolume]);
			label[FreeLabel]->setVisible(true);
			Ball.sprite->setVisible(false);
			Ball.pos = Vec2(270.0f, 840.0f);
			Ball.spd = Vec2(0.0f, 0.0f);
			ReBall[Ball.cnt].type = Probability();
			ReBall[Ball.cnt].sprite->setVisible(true);	
			Chucker[i].cnt = 1;
			Playing = false;			
			LabelTime = 40;
			Ball.cnt++;		
			BallStreak->setVisible(false);
		}		
		//�����\��
		if (label[FreeLabel]->isVisible() && Chucker[i].cnt == 1)
			label[FreeLabel]->setPosition(Vec2(Chucker[i].pos.x, Chucker[i].pos.y + 30.0f));
	}
}
// �����X�V���� =============================================
void Play::LabelUpdate()
{	
	String *BallCnt = String::createWithFormat("%d", Ball.cnt);
	label[CountLabel]->setString(BallCnt->getCString());
	String *CombCnt = String::createWithFormat("%d Comb", Comb);
	label[CombLabel]->setString(CombCnt->getCString());
	label[CombLabel]->setPosition(Vec2(Ball.pos.x, Ball.pos.y + 25.0f));
	label[BonusLabel]->setPosition(Vec2(Ball.pos.x, Ball.pos.y + 50.0f));	
	if (Comb == 0 || Ball.pos.y >= 800.0f || Ball.pos.y <= 110.0f)
	{
		label[CombLabel]->setVisible(false);
		label[BonusLabel]->setVisible(false);
	}
	else label[CombLabel]->setVisible(true);
	if (Comb % 20 == 0 && Comb != 0)
	{
		if (Ball.type <= 4)
		{
			int id = AudioEngine::play2d("Sound/VOICE/LevelUp.mp3");
			AudioEngine::setVolume(id, 0.3f);
		}
		label[CombLabel]->setColor(Color3B::RED);
		label[BonusLabel]->setVisible(true);		
		LabelTime = 40;
		Ball.type++;
		Comb += 2;		
	}	
	static int CombNum;
	if (Comb % 20 != 0)
	{
		if (Comb <= 20)
			CombNum = Comb;
		else CombNum = Comb - (Comb / 20 * 20);
		CombNum *= (540 / 20);
	}
	else CombNum = 0;
	switch (Ball.type)
	{
	case 0:CombGauge->setColor(Color3B::WHITE); break;
	case 1:CombGauge->setColor(Color3B::GREEN); break;
	case 2:CombGauge->setColor(Color3B::RED); break;
	case 3:CombGauge->setColor(Color3B::YELLOW); break;
	default:CombGauge->setColor(Color3B::MAGENTA); break;
	}
	if (Ball.type >= 4)
		CombGauge->setTextureRect(Rect(0.0f, 0.0f, 540.0f, 60.0f));
	else CombGauge->setTextureRect(Rect(0.0f, 0.0f, CombNum, 60.0f));
	LabelTime--;
	if (LabelTime <= 0)
	{
		for (int i = 0; i < 2; i++)
		{
			label[i]->setVisible(false);
			Chucker[i].cnt = 0;
		}
	}
}
// �Q�[�g�����ʒu ===========================================
void Play::GatePosition(){
	srand((unsigned int)time(nullptr));
	int GatePos[2];	
	for (int i = 0; i < 2; i++){
		GatePos[i] = rand() % 14;
		while (GatePos[1] == GatePos[0])
			GatePos[1] = rand() % 14;
		switch (GatePos[i]){
		case 0:	Gate[i].sprite->setPosition(Vec2(180.0f, 760.0f)); break;
		case 1:	Gate[i].sprite->setPosition(Vec2(270.0f, 540.0f)); break;
		case 2:	Gate[i].sprite->setPosition(Vec2(270.0f, 600.0f)); break;
		case 3:	Gate[i].sprite->setPosition(Vec2(50.0f, 680.0f)); break;
		case 4:	Gate[i].sprite->setPosition(Vec2(70.0f, 320.0f)); break;
		case 5:	Gate[i].sprite->setPosition(Vec2(180.0f, 240.0f)); break;
		case 6:	Gate[i].sprite->setPosition(Vec2(170.0f, 380.0f)); break;
		case 7: Gate[i].sprite->setPosition(Vec2(470.0f, 660.0f)); break;
		case 8:	Gate[i].sprite->setPosition(Vec2(360.0f, 760.0f)); break;
		case 9:	Gate[i].sprite->setPosition(Vec2(490.0f, 680.0f)); break;
		case 10: Gate[i].sprite->setPosition(Vec2(470.0f, 320.0f)); break;
		case 11: Gate[i].sprite->setPosition(Vec2(360.0f, 240.0f)); break;
		case 12: Gate[i].sprite->setPosition(Vec2(370.0f, 380.0f)); break;
		case 13: Gate[i].sprite->setPosition(Vec2(70.0f, 660.0f)); break;
		}
	}
}
// �Q�[���I�[�o�[�X�V���� ===================================
void Play::GameOverUpdate()
{
	if (Playing != true && Ball.cnt == 0 &&
		GameOver->isVisible() != true && Attack.type == 0 && HP>0)
	{
		GameOver->setVisible(true);
		for (int i = 0; i < 2; i++)
		{
			Button[i]->setPosition(Vec2(200.0f + i * 140, 400.0f));			
			Button[i]->setVisible(true);
		}
		AudioEngine::pause(BGM);
		int id = AudioEngine::play2d("Sound/SE/GameOver.mp3");
		AudioEngine::setVolume(id, 0.4f);
	}
}
// �Q�[���N���A�X�V���� =====================================
void Play::GameClearUpdate()
{	
	if (HP <= 0 && Attack.type == 0 &&GameClear[0]->isVisible()==false){
		AudioEngine::stop(BGM);
		int id = AudioEngine::play2d("Sound/SE/Clear.mp3");
		AudioEngine::setVolume(id, Volume[ObjectVolume]);
		int id2 = AudioEngine::play2d("Sound/SE/Clear2.mp3");
		AudioEngine::setVolume(id2, 0.3f);
		for (int i = 0; i < 2;i++)
		GameClear[i]->setVisible(true);
		Button[0]->setPosition(Vec2(270.0f, 350.0f));
		Button[0]->setVisible(true);
	}	
	if (GameClear[0]->isVisible())	
	{
		srand((unsigned int)time(nullptr));
		static int time;
		if (time % (rand()%70+30) == 0)
		{
			for (int i = 0; i < rand() % 8 + 1; i++)
			{
				ParticleExplosion* explosion;
				explosion = ParticleExplosion::create();
				explosion->setPosition(Vec2(rand() % 420 + 60, rand() % 780 + 60));
				explosion->setSpeed(100);
				explosion->setLife(0.4f);
				explosion->setStartColor(Color4F::ORANGE);
				this->addChild(explosion,8);
				switch (rand() % 7){
				case 0:break;
				case 1:explosion->setStartColor(Color4F::RED); break;
				case 2:explosion->setStartColor(Color4F::ORANGE); break;
				case 3:explosion->setStartColor(Color4F::GREEN); break;
				case 4:explosion->setStartColor(Color4F::MAGENTA); break;
				case 5:explosion->setStartColor(Color4F::YELLOW); break;
				case 6:explosion->setStartColor(Color4F::BLUE); break;
				}				
				time = 0;
			}		
			int id = AudioEngine::play2d("Sound/SE/Explosion.mp3");
			AudioEngine::setVolume(id, 0.5f);
		}
		time++;
	}
}
// �G�t�F�N�g�X�V���� =======================================
void Play::EffectUpdate()
{	
	Cannon.cnt++;
	if (Playing == true && Cannon.cnt == 12)
		Effect->setVisible(true);
	else if (Effect->isVisible())
	{		
		if (Cannon.cnt % 2 == 0)
			Cannon.type++;
		if (Cannon.type == 11)
			Effect->setVisible(false);
		char s[128];
		sprintf(s, "Effect/Effect(%d).png", Cannon.type);		
		Effect->setTexture(s);		
	}
}
// �G�A�j���[�V���� =========================================
void Play::EnemyAnimation(){
	Enemy.cnt++;
	if (Enemy.cnt % 20 == 0){
		if (Enemy.type == 0)
			Enemy.type = 1;
		else Enemy.type = 0;
	}
	switch (g_data.stage){
	case 0:
		switch (Enemy.type){
		case 0: Enemy.sprite->setTexture("Chara/Enemy/Angel.png"); break;
		case 1: Enemy.sprite->setTexture("Chara/Enemy/Angel2.png"); break;
		}
		break;
	case 1:
		switch (Enemy.type){
		case 0: Enemy.sprite->setTexture("Chara/Enemy/Synchro.png"); break;
		case 1: Enemy.sprite->setTexture("Chara/Enemy/Synchro2.png"); break;
		}
		break;
	}
}
// �}�b�v�`�b�v�f�[�^ =======================================
void Play::MapChipData(int map[][26])
{
	//�I�u�W�F�N�g�̃X�v���C�g
	Sprite* Wall[OBJECTNUM];
	for (int i = 0; i < OBJECTNUM; i++)
	{
		Peg[i].sprite = Sprite::create("Object/Peg.png");
		Box[i].sprite = Sprite::create("Object/Box.png");
		Wall[i] = Sprite::create("Object/Wall.png");
		this->addChild(Peg[i].sprite, 1);
		this->addChild(Box[i].sprite, 1);
		this->addChild(Wall[i], 1);
	}
	int number[3];
	for (int i = 0; i < 3; i++)
		number[i] = 0;
	for (int i = 0; i < 35; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			if (map[i][j] == 1)
			{
				Peg[number[0]].sprite->setPosition(
					Vec2(10.0 + j*20.0f, 790 - i*20.0f));
				number[0]++;
			}
			if (map[i][j] == 2)
			{
				Box[number[1]].sprite->setPosition(
					Vec2(10.0 + j*20.0f, 790 - i*20.0f));
				number[1]++;
			}
			if (map[i][j] == 3)
			{
				Wall[number[2]]->setPosition(
					Vec2(10.0 + j*20.0f, 790 - i*20.0f));
				number[2]++;
			}
		}
	}
}
// �}�b�v�쐬 ===============================================
void Play::MapChipCreate()
{
	//�}�b�v�`�b�v
	switch (g_data.stage)
	{
	case 0:	
	{
				int MapChip[35][26] = 
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2, 0, 0, 0, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
				};
				MapChipData(MapChip);
				break;
	}
	case 1:	
	{
				int MapChip[35][26] = 
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2, 0, 0, 0, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
				};
				MapChipData(MapChip);
				break;
	}
	case 2:	
	{
				int MapChip[35][26] = 
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 2, 0, 0, 0, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
				};
				MapChipData(MapChip);
				break;
	}
	}
}
// �p�[�e�B�N���쐬 =========================================
void Play::ParticleCreate(
	Vec2 gravity, float speed, Vec2 position)
{
	particle = ParticleMeteor::create();
	particle->setScale(0.5f);	
	particle->setGravity(gravity);
	particle->setSpeed(speed);
	this->addChild(particle, 3);
	particle->setPosition(position);
	particle->setDuration(0.5f);
}
// �����쐬 =================================================
void Play::LabelCreate()
{
	//����
	label[FreeLabel] = Label::createWithSystemFont("FreeBall", "arial", 20);
	label[BonusLabel] = Label::createWithSystemFont("�{�[�i�X!�I +2Comb", "arial", 20);
	label[BonusLabel]->setColor(Color3B::YELLOW);
	label[CountLabel] = Label::createWithSystemFont("", "arial", 30);
	label[CountLabel]->setPosition(Vec2(515.0f, 850.0f));
	label[CombLabel] = Label::createWithSystemFont("", "arial", 20);
	label[CombLabel]->setColor(Color3B::YELLOW);
	label[PauseLabel] = Label::createWithSystemFont("�|�[�Y", "arial", 30);
	label[PauseLabel]->setPosition(Vec2(270.0f, 700.0f));
	label[AttackLabel] = Label::createWithSystemFont("", "arial", 20);
	label[AttackLabel]->setPosition(Vec2(270.0f, 200.0f));
	label[HPLabel] = Label::createWithSystemFont("", "arial", 20);
	label[HPLabel]->setPosition(Vec2(270.0f, 360.0f));
	label[TouchLabel] = Label::createWithSystemFont("", "arial", 40);
	label[TouchLabel]->setPosition(Vec2(460.0f, 40.0f));
	label[UpdateLabel] = Label::createWithSystemFont("", "arial", 18);
	label[UpdateLabel]->setPosition(Vec2(10.0f, 105.0f));
	label[UpdateLabel]->setAnchorPoint(Vec2(0.0f, 1.0f));
	label[UpdateLabel]->setColor(Color3B::YELLOW);
	label[UpdateLabel]->setVisible(false);
	for (int i = 0; i < 5; i++)
		label[i]->setVisible(false);
	label[HPLabel]->setVisible(true);
	for (int i = 0; i < LabelNum; i++)
		addChild(label[i], 4);
}
// �X�v���C�g�쐬 ===========================================
void Play::SpriteCreate()
{
	//�摜�ǂݍ���
	Ball.sprite = Sprite::create("Object/Ball.png");
	Ball.sprite->setPosition(Vec2(270.0f, 840.0f));
	this->addChild(Ball.sprite, 2);
	//�c��
	BallStreak = MotionStreak::create(0.3f, 1.0f, 25.0f, Color3B::WHITE, "Effect/Effect(3).png");
	this->addChild(BallStreak, 2);
	//�Q�[�g�̃X�v���C�g
	Gate[0].sprite = Sprite::create("Object/Gate.png");
	Gate[1].sprite = Sprite::create("Object/Gate2.png");
	for (int i = 0; i < 2; i++){
		auto RotaRepeat = RepeatForever::create(RotateBy::create(1.0f, 360.0f));
		Gate[i].sprite->runAction(RotaRepeat);
		this->addChild(Gate[i].sprite, 1);
	}
	if(g_data.stage!=2)
	GatePosition();
	switch (g_data.stage){
	case 0:	Character = Sprite::create("Chara/Player.png"); break;
	case 1:	Character = Sprite::create("Chara/Player.png"); break;
	case 2:Character = Sprite::create("Chara/Player.png"); break;
	}
	Character->setPosition(Vec2(270.0f, 845.0f));
	this->addChild(Character, 6);
	////�w�i�̃X�v���C�g
	//auto background = Sprite::create("BackGround.png");
	//background->setPosition(Vec2(270.0f, 450.0f));
	//this->addChild(background, 0);
	//�G�̃X�v���C�g
	switch (g_data.stage){
	case 0:	Enemy.sprite = Sprite::create("Chara/Enemy/Angel2.png"); break;
	case 1:	Enemy.sprite = Sprite::create("Chara/Enemy/Synchro2.png"); break;
	case 2: Enemy.sprite = Sprite::create("Chara/Enemy/Synchro2.png"); break;
	}
	Enemy.sprite->setPosition(Vec2(270.0f, 295.0f));
	Enemy.sprite->setVisible(true);
	this->addChild(Enemy.sprite, 1);
	if (g_data.stage == 2)
		Enemy.sprite->runAction(
			RepeatForever::create(
				JumpTo::create(2.0f, Vec2(270.0f, 295.0f), 40.0f, 1)));
	//��C�̃X�v���C�g
	Cannon.sprite = Sprite::create("Object/Cannon.png");
	Cannon.sprite->setAnchorPoint(Vec2(0.5f, 1.7f));
	Cannon.sprite->setPosition(Vec2(270.0f, 840.0f));
	this->addChild(Cannon.sprite, 3);
	//�O�i�̃X�v���C�g
	auto frontground = Sprite::create("Back/PlayScene/FrontGround.png");
	frontground->setPosition(Vec2(270.0f, 450.0f));
	this->addChild(frontground, 4);
	//�U���`���b�J�[�̃X�v���C�g
	auto chucker = Sprite::create("Object/AttackChuker.png");
	chucker->setPosition(Vec2(270.0f, 115.0f));
	this->addChild(chucker, 1);
	//�K�E�Z�̃X�v���C�g
	switch (g_data.stage)
	{
	case 0:	Special = Sprite::create("Special.png"); break;
	case 1:	Special = Sprite::create("Special.png"); break;
	case 2: Special = Sprite::create("Special.png"); break;
	}
	Special->setPosition(Vec2(270.0f, 530.0f));
	Special->setVisible(false);
	this->addChild(Special, 3);
	//�Q�[���I�[�o�[�̃X�v���C�g
	GameOver = Sprite::create("UI/GameOver.png");
	GameOver->setPosition(Vec2(270.0f, 450.0f));
	this->addChild(GameOver, 5);
	//�Q�[���N���A�̃X�v���C�g
	GameClear[0] = Sprite::create("UI/Clear.png");
	GameClear[0]->setPosition(Vec2(270.0f, 600.0f));
	GameClear[1] = Sprite::create("UI/Clear2.png");
	GameClear[1]->setPosition(Vec2(270.0f, 400.0f));
	for (int i = 0; i < 2; i++)
	{
		GameClear[i]->setVisible(false);
		this->addChild(GameClear[i], 5);
	}
	//�{�^���̃X�v���C�g
	Button[0] = Sprite::create("UI/Button.png");
	Button[1] = Sprite::create("UI/Button2.png");
	Button[2] = Sprite::create("UI/Button3.png");
	Button[2]->setPosition(Vec2(70.0f, 870.0f));
	for (int i = 0; i < 2; i++)
		this->addChild(Button[i], 6);
	this->addChild(Button[2], 4);
	//�c��{�[���̃X�v���C�g
	for (int i = 0; i < BALLNUM; i++)
	{
		ReBall[i].sprite = Sprite::create("Object/Ball.png");
		this->addChild(ReBall[i].sprite, 4);
	}
	//�`���b�J�[�̃X�v���C�g
	for (int i = 0; i < 2; i++)
	{
		Chucker[i].sprite = Sprite::create("Object/Chucker.png");
		Chucker[i].sprite->setPosition(Vec2(115.0f + (i*310.0f), 115.0f));
		this->addChild(Chucker[i].sprite, 0);
	}
	Chucker[0].spd = Vec2(1.0f, 0.0f);
	Chucker[1].spd = Vec2(-1.0f, 0.0f);
	CombGauge = Sprite::create("UI/Comb.png");
	CombGauge->setAnchorPoint(Vec2(0.0f, 0.5f));
	CombGauge->setPosition(Vec2(0.0f, 70.0f));
	this->addChild(CombGauge, 7);
	// �G�t�F�N�g
	Effect = Sprite::create();
	Effect->setAnchorPoint(Vec2(0.5f, 2.9f));
	Effect->setPosition(Vec2(270.0f, 840.0f));
	Effect->setVisible(false);
	addChild(Effect, 2);
	// �p���[���[�^�[
	Power = Sprite::create("UI/Power.png");
	Power->setPosition(Vec2(330.0f, 895.0f));
	Power->setAnchorPoint(Vec2(0.0f, 1.0f));
	addChild(Power, 9);
}
// ���쐬 ===================================================
void Play::BGMCreate(){
	Volume[BGMVolume] = 0.3f;
	Volume[BallVolume] = 0.1f;
	Volume[ObjectVolume] = 0.5f;
	Volume[GateVolume] = 0.6f;
	char s[128];
	sprintf(s, "Sound/BGM/BGM(%d).mp3", g_data.selectBGM);
	BGM = AudioEngine::play2d(s,true);
	AudioEngine::setLoop(BGM, true);
}
void Play::CollisionC2(){
	Ball.spd = Vec2(Ball.spd.x, -Ball.spd.y);
}