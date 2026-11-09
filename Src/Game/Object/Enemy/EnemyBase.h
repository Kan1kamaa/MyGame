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

protected:
	bool m_isDying;		//死亡モーション再生中かどうか
	VECTOR m_speed;
	Status m_status;
	int m_flashCnt;		//TakeDamageで被弾したとき、赤く光らせる残りフレーム数

	//基底で毎フレーム更新するもの(属性の残り時間、被弾の光)をまとめて進める
	//派生クラスのStepから1回呼ぶ
	void StepBase();
	void StepElement();

	unsigned int GetElementColor();

};