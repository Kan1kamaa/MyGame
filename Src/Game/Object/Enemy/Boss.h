#pragma once
#include"../ActorBase/ActorBase.h"
#include"../../System/Status.h"

class BossGolem : public ActorBase {
private:
	VECTOR m_speed;			//移動速度
	bool m_isDying;			//死亡モーション再生中かどうか
	int m_attackCoolCnt;	//次の攻撃を出すまでのフレーム数

	enum EnemyState
	{
		Search,
		Chase,
		Attack
	};
	EnemyState m_state;	//現在の行動状態

	//BossGolemモデルに書き出されているアニメーションの並び順
	enum AnimID
	{
		ANIM_ATTACK1,
		ANIM_ATTACK2,
		ANIM_ATTACK3,
		ANIM_DEATH,
		ANIM_DOWN,
		ANIM_IDLE,
		ANIM_JUMPATTACK,
		ANIM_ROCKPILLAR,
		ANIM_ROLLINGATTACK,
		ANIM_THROWROCK,
		ANIM_WALK,
	};

	Status m_status;

	//moveDirの方向へ少しずつ向き直る
	void TurnToward(const VECTOR& moveDir);
	//プレイヤーが索敵範囲外にいるときはその場で待機する
	void StepSearch();
	//プレイヤーを追いかける
	void StepChase(const VECTOR& playerPos);
	//プレイヤーを攻撃する
	void StepAttack(const VECTOR& playerPos);
public:
	//コンストラクタ・デストラクタ
	BossGolem();
	~BossGolem();

	//初期化
	void Init();
	//ロード
	void Load();
	//毎フレーム計算する処理
	//@playerPos : プレイヤーの現在座標(索敵・追跡・攻撃の判定に使う)
	void Step(const VECTOR& playerPos);
	//出現させる
	//@pos : 出現させる座標
	//@return : true = 成功  false = 失敗(既に出現中)
	bool Request(const VECTOR& pos);

	void HitCalc(const ObjectBase& other);

	float GetAttackPower()const { return m_status.AttackPower; }
};
