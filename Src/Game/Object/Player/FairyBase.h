#pragma once
#include "../ActorBase/ActorBase.h"
#include"math.h"
#include"../../../Lib/Effect/EffectManager.h"
#include"../../System/Element.h"
const VECTOR FAIRY_POS = { -15,10,-15 };	//精霊がプレイヤーからどれだけ離れているか

//精霊の基底クラス(風・炎・雷の精霊はこれを継承する)
class FairyBase : public ActorBase {

protected:
	enum FairyAnumID
	{
		ANIMID_ATTACK,
		ANIMID_IDLE,
		ANIMID_MOVE
	};
	enum  FairyState {
		FOLLOW,
		ATTACK,
		SKILL,
		END,

		STATENUM,
	};

	int m_hitCnt;
	int m_effectRequest;
	FairyState m_state;
	ElementType m_element;
	VECTOR m_SkillCenter;
	float m_SkillRotY;
	int m_SkillMinFrame;
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
	virtual void Step(const VECTOR& playerPos,float rotY);
	//情報更新
	void Update() override;
	//描画
	virtual void Draw();
	//破棄
	virtual void Fin();

	//攻撃を開始する(属性ごとに中身を変えるので、各精霊クラスで実装する)
	virtual void StartAttack(const VECTOR& playerPos,float playerRotY) = 0;

	//モデルを持たせて、待機アニメをループ再生する(Initの後に呼ぶ)
	void SetModel(int hndl);

	bool IsSkillFinished() const;

	void isLeave();

	bool isEnd() const;

	bool PopEffectRequest(EffectManager::EffectId& id);

	//スキルの判定を出しているか(GameCollisionから使う。属性ごとに上書きする)
	virtual bool IsSkillActive()const
	{
		return false;
	}

	//スキルの判定(球)の中心座標
	virtual VECTOR GetSkillPos()const
	{
		return m_SkillCenter;
	}

	//スキルの判定(球)の半径
	virtual float GetSkillRadius()const
	{
		return 0.0f;
	}

	//スキルの強さ(風なら引き寄せる強さ、ほかの属性ならダメージなど)
	virtual float GetSkillPower()const
	{
		return 0.0f;
	}

	virtual float GetSkillRotY()const
	{
		return m_SkillRotY;
	}

	virtual bool IsSkillHit()const
	{
		return false;
	}

	ElementType GetFairyElement()const
	{
		return m_element;
	}
};
