#pragma once
#include "../ActorBase/ActorBase.h"

//精霊の基底クラス(風・炎・雷の精霊はこれを継承する)
class FairyBase : public ActorBase {
private:

	enum  FairyState {
		FOLLOW,
		ATTACK,

		STATENUM,
	};

	VECTOR m_TargetPos;
	float m_MoveSpeed;
	float m_FlowTimer;
	int m_AttackCount;
	FairyState m_state;

public:
	//コンストラクタ・デストラクタ
	FairyBase();
	virtual ~FairyBase();

	//初期化
	void Init() override;
	//毎フレーム計算する処理
	//@playerPos : ついていく相手(プレイヤー)の座標
	virtual void Step(const VECTOR& playerPos);
	//情報更新
	void Update() override;
	//描画
	virtual void Draw();
	//破棄
	virtual void Fin();

	//攻撃を開始する(属性ごとに中身を変えるので、各精霊クラスで実装する)
	virtual void StartAttack() = 0;
};
