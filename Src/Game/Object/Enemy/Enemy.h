#pragma once
#include"../ActorBase/ActorBase.h"
#include"../../System/Status.h"
class Enemy : public ActorBase {
private:
	VECTOR m_speed;		//移動速度
	int m_changeDirCnt;	//次にランダムで方向転換するまでのフレーム数
	bool m_isDying;		//死亡モーション再生中かどうか
	//被弾後、無敵時間として残っているフレーム数(この間はHitCalcでダメージを受けない)
	int m_invincibleCnt;
	enum EnemyState
	{
		Search,
		Chase,
		Attack
	};
	EnemyState m_state;	//現在の行動状態

	//Golemモデルに書き出されているアニメーションの並び順
	enum AnimID
	{
		ANIM_ATTACK,
		ANIM_DEATH,
		ANIM_WALK,
	};

	Status m_status;

	//移動方向をランダムに選び直す
	void RandomizeDirection();
	//プレイヤーを見失っているときのランダム徘徊
	void StepSearch();
	//プレイヤーを追いかける
	void StepChase(const VECTOR& playerPos);
	//プレイヤーを攻撃する
	void StepAttack(const VECTOR& playerPos);
	//moveDirの方向へ少しずつ向き直る
	void TurnToward(const VECTOR& moveDir);
public:
	//コンストラクタ・デストラクタ
	Enemy();
	~Enemy();

	//初期化
	void Init();
	//ロード
	void Load(int originhndl);
	//毎フレーム計算する処理
	//@playerPos : プレイヤーの現在座標(索敵・追跡・攻撃の判定に使う)
	void Step(const VECTOR& playerPos);
	//ショット発射
	//@pos : 発射する座標
	//@speed :　移動速度
	//@return : true = 生成成功 false = 失敗
	bool Request(const VECTOR& pos, const VECTOR& speed);

	void HitCalc(const ObjectBase& other);

	float GetAttackPower()const { return m_status.AttackPower; }
};
