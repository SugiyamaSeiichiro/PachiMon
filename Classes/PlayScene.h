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
		cocos2d::Sprite* sprite;//スプライト
		cocos2d::Vec2 pos;		//座標			
		cocos2d::Vec2 spd;		//速度	
		float angle;			//角度		
		int cnt;				//カウント		
		int type;				//種類
	};	
	// 発射用のボール
	object Ball;	
	// 残りのボール
	object ReBall[BALLNUM];
	// 釘
	object Peg[OBJECTNUM];
	// 箱
	object Box[OBJECTNUM];	
	// 攻撃
	object Attack;
	// 大砲
	object Cannon;	
	// チャッカー
	object Chucker[2];	
	// 初期処理
	virtual bool init();
	// 初期化
	void initializ();	
	// 更新処理
	void update(float delta);	
	// 保留色
	void HoldColor(int type,cocos2d::Sprite* sprite);	
	// 保留色確率
	int Probability();
	// チャッカー更新処理
	void ChuckerUpdate();
	void LabelCreate();
	void SpriteCreate();
	void BGMCreate();
	// 文字更新処理
	void LabelUpdate();
	// オブジェクト更新処理
	void ObjectCollision();
	// 四角同士の当たり判定
	bool CollisionS(cocos2d::Vec2 square, cocos2d::Vec2 square2, cocos2d::Vec2 radius2);
	// 円同士の当たり判定
	bool CollisionC(cocos2d::Vec2 circle, cocos2d::Vec2 circle2, float radius2);
	// 物理的反射
	cocos2d::Vec2 Reflection(object fall, cocos2d::Vec2 obstacle);
	// タッチ開始
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	// タッチ移動
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	// タッチ終了
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);	
	// タッチフラグ
	bool TouchFlag;
	// タッチカウンター
	int TouchCnt;
	// 当たり判定カウンター
	int CollisionCnt;	
	//コンボ数
	int Comb;
	// ゲームオーバースプライト
	cocos2d::Sprite* GameOver;
	cocos2d::Sprite* GameClear[2];
	// 必殺スプライト
	cocos2d::Sprite* Special;
	// ボタンスプライト
	cocos2d::Sprite* Button[3];	
	//BGM
	int BGM;
	//文字
	cocos2d::Label* label[LabelNum];
    // 文字描画時間
	int LabelTime;		
	// 敵HP
	int HP;
	// ゲート
	object Gate[2];
	// エフェクト
	cocos2d::Sprite* Effect;
	void EffectUpdate();
	object Enemy;
	void EnemyAnimation();
	// プレイフラグ
	bool Playing;
	// ゲート発生位置
	void GatePosition();
	// ゲート更新処理
	void GateUpdate();
	// 攻撃更新処理
	void AttackUpdate();
	// ゲームオーバー
	void GameOverUpdate();
	// ゲームクリア
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
	//プレイシーン作成
	static cocos2d::Scene* createScene();
};
#endif