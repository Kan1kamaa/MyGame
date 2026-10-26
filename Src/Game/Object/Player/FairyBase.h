#pragma once
#include "../ActorBase/ActorBase.h"
#include "../../../Lib/Effect/EffectManager.h"

//精霊の属性
enum class FairyElement {
	Wind,
	Fire,
	Thunder,
};

//精霊の状態
enum class FairyState {
	Summon,		//出現中
	Follow,		//プレイヤーについてくる
	Attack,		//攻撃中(まだプレイヤーについてくる)
	Leaving,	//切り替え済み。その場に留まって攻撃を出し切る
	Vanish,		//消滅中
};

//精霊の基底クラス(属性ごとの精霊はこれを継承する)
class FairyBase : public ActorBase {
protected:
	FairyElement m_element;		//属性
	FairyState   m_state;		//現在の状態
	int          m_stateCnt;	//今の状態になってからのフレーム数

	VECTOR m_followOffset;		//プレイヤーからの目標相対位置
	float  m_followSpeed;		//追従の速さ(0〜1。大きいほどすぐ追いつく)
	float  m_floatTimer;		//ふわふわ揺れる用のタイマー

	bool   m_isAttacking;		//攻撃を出している最中か(Leavingから消えてよいかの判定に使う)
	float  m_attackPower;		//攻撃力(この精霊自身の値を使う)

public:
	FairyBase();
	virtual ~FairyBase();

	//初期化
	void Init() override;
	//毎フレーム計算する処理
	//@playerPos : 追従する相手(プレイヤー)の座標
	virtual void Step(VECTOR playerPos);
	//情報更新
	void Update() override;
	//描画
	virtual void Draw();
	//破棄
	virtual void Fin();

	//Eスキルの攻撃を開始する(属性ごとに実装)
	virtual void StartAttack() = 0;

	//切り替えられた時に呼ぶ。以降は追従せず、攻撃を出し切ったら消える
	void Leave();
	//消えてよいか(削除対象か)
	bool IsFinished() const;

	//追従する位置(プレイヤーからの相対座標)を変える
	void SetFollowOffset(VECTOR offset) { m_followOffset = offset; }

	//=========================
	//攻撃の当たり判定(GameCollisionから使う)
	//=========================
	virtual bool IsAttackActive() const { return false; }
	virtual VECTOR GetAttackPos() const { return m_pos; }
	virtual float GetAttackRadius() const { return 0.0f; }
	float GetAttackPower() const override { return m_attackPower; }

	FairyElement GetElement() const { return m_element; }
	FairyState GetState() const { return m_state; }

protected:
	//状態を変える(m_stateCntを0に戻す)
	void ChangeState(FairyState next);
	//属性ごとの攻撃の毎フレーム処理(Attack / Leaving 中に呼ばれる)
	virtual void StepAttack() {}
	//プレイヤーの目標位置へ滑らかに近づく
	void FollowPlayer(VECTOR playerPos);
};
