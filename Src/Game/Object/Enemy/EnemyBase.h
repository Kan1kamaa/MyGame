#pragma once
#include"../ActorBase/Actorbase.h"
#include"../../System/Element.h"
#include"../../System/Status.h"
class EnemyBase : public ActorBase
{
private:

	ElementType m_element;
	int m_elementCnt;
	
public:
	EnemyBase();
	~EnemyBase();

	void Init();

	void AddElement(ElementType);

	ElementType GetElement()const;

	virtual void Pull(const VECTOR& center, float power);

	void TakeDamage(int damage);

	virtual void OnDead();

	void ClearElement();

	void StartShock();

	bool IsShocked()const;

	virtual bool IsStunned()const;

	bool RollShockStun()const;

	//=========================
	//攻撃の当たり判定(GameCollisionから使う。敵の種類ごとに上書きする)
	//=========================
	//攻撃判定を出しているか(攻撃モーションの振り下ろしの区間だけtrue)
	virtual bool IsAttackActive()const { return false; }
	//攻撃判定(球)の中心座標
	virtual VECTOR GetAttackPos()const { return m_pos; }
	//攻撃判定(球)の半径
	virtual float GetAttackRadius()const { return 0.0f; }
protected:
	bool m_isDying;		//死亡モーション再生中かどうか
	VECTOR m_speed;
	Status m_status;
	int m_flashCnt;		//TakeDamageで被弾したとき、赤く光らせる残りフレーム数
	int m_shockCnt;
	//基底で毎フレーム更新するもの(属性の残り時間、被弾の光)をまとめて進める
	//派生クラスのStepから1回呼ぶ
	void StepBase();
	void StepElement();

	unsigned int GetElementColor();



};