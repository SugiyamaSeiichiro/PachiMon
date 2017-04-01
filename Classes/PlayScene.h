#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__
#include "cocos2d.h"
#include "ui\CocosGUI.h"
const int BALLNUM = 5;
const int OBJECTNUM = 25;
class Play :public cocos2d::Layer
{
private:	
	enum label{
		FreeLabel,	
		BonusLabel,
		AttackLabel,
		HPLabel,
		PauseLabel,		
		CountLabel,
		CombLabel,
		TouchLabel,
		UpdateLabel,
		LabelNum,
	};	
	enum volume{
		BallVolume,
		BGMVolume,
		ObjectVolume,
		GateVolume,
		VolumeNum,
	};
	struct object{
		cocos2d::Sprite* sprite;//�X�v���C�g
		cocos2d::Vec2 pos;		//���W			
		cocos2d::Vec2 spd;		//���x	
		float angle;			//�p�x		
		int cnt;				//�J�E���g		
		int type;				//���
	};	
	// ���˗p�̃{�[��
	object Ball;	
	// �c��̃{�[��
	object ReBall[BALLNUM];
	// �B
	object Peg[OBJECTNUM];
	// ��
	object Box[OBJECTNUM];	
	// �U��
	object Attack;
	// ��C
	object Cannon;	
	// �`���b�J�[
	object Chucker[2];	
	// ��������
	virtual bool init();
	// ������
	void initializ();	
	// �X�V����
	void update(float delta);	
	// �ۗ��F
	void HoldColor(int type,cocos2d::Sprite* sprite);	
	// �ۗ��F�m��
	int Probability();
	// �`���b�J�[�X�V����
	void ChuckerUpdate();
	void LabelCreate();
	void SpriteCreate();
	void BGMCreate();
	// �����X�V����
	void LabelUpdate();
	// �I�u�W�F�N�g�X�V����
	void ObjectCollision();
	// �l�p���m�̓����蔻��
	bool CollisionS(cocos2d::Vec2 square, cocos2d::Vec2 square2, cocos2d::Vec2 radius2);
	// �~���m�̓����蔻��
	bool CollisionC(cocos2d::Vec2 circle, cocos2d::Vec2 circle2, float radius2);
	// �����I����
	cocos2d::Vec2 Reflection(object fall, cocos2d::Vec2 obstacle);
	// �^�b�`�J�n
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	// �^�b�`�ړ�
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	// �^�b�`�I��
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);	
	// �^�b�`�t���O
	bool TouchFlag;
	// �^�b�`�J�E���^�[
	int TouchCnt;
	// �����蔻��J�E���^�[
	int CollisionCnt;	
	//�R���{��
	int Comb;
	// �Q�[���I�[�o�[�X�v���C�g
	cocos2d::Sprite* GameOver;
	cocos2d::Sprite* GameClear[2];
	// �K�E�X�v���C�g
	cocos2d::Sprite* Special;
	// �{�^���X�v���C�g
	cocos2d::Sprite* Button[3];	
	//BGM
	int BGM;
	//����
	cocos2d::Label* label[LabelNum];
    // �����`�掞��
	int LabelTime;		
	// �GHP
	int HP;
	// �Q�[�g
	object Gate[2];
	// �G�t�F�N�g
	cocos2d::Sprite* Effect;
	void EffectUpdate();
	object Enemy;
	void EnemyAnimation();
	// �v���C�t���O
	bool Playing;
	// �Q�[�g�����ʒu
	void GatePosition();
	// �Q�[�g�X�V����
	void GateUpdate();
	// �U���X�V����
	void AttackUpdate();
	// �Q�[���I�[�o�[
	void GameOverUpdate();
	// �Q�[���N���A
	void GameClearUpdate();	
	int ChangeSceneTime;	
	cocos2d::MotionStreak* Streak;
	cocos2d::MotionStreak* BallStreak;
	cocos2d::Sprite* Power;
	cocos2d::Sprite* Character;
	float Meter = 0.0f;	
	float Volume[VolumeNum];
	void MapChipData(int map[][26]);
	void MapChipCreate();
	cocos2d::ParticleMeteor* particle;	
	void ParticleCreate(cocos2d::Vec2 gravity, float speed, cocos2d::Vec2 position);
	cocos2d::ParticleSun* Fire;
	int SoundTime;
	cocos2d::Sprite* CombGauge;
	void CollisionC2();
	CREATE_FUNC(Play);
public:
	//�v���C�V�[���쐬
	static cocos2d::Scene* createScene();
};
#endif