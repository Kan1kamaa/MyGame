#pragma once
#include "../ActorBase/ActorBase.h"

const VECTOR FAIRY_POS = { -30,10,-30 };	//精霊がプレイヤーからどれだけ離れているか

//精霊の基底クラス(風・炎・雷の精霊はこれを継承する)
class FairyBase : public ActorBase {

protected:
	enum  FairyState {
		FOLLOW,
		ATTACK,
		SKILL,
		END,

		STATENUM,
	};
	FairyState m_state;
private:
	VECTOR m_TargetPos;
	float m_MoveSpeed;
	float m_FlowTimer;
	
	bool m_isLeaving;
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

	//モデルを持たせて、待機アニメをループ再生する(Initの後に呼ぶ)
	void SetModel(int hndl);

	bool IsSkillFinished() const;

	void isLeave();

	bool isEnd() const;
};
