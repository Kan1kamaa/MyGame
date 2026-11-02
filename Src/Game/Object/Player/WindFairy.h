#pragma once
#include "FairyBase.h"

//風の精霊
class WindFairy : public FairyBase {
private:
	VECTOR m_SkillCenter;
public:
	WindFairy();

	//風の攻撃を開始する
	void StartAttack(const VECTOR& playerPos,float playerRotY) override;

	bool IsPullActive()const override;

	VECTOR GetPullPos()const override;

	float GetPullRadius()const override;

	float GetPullPower()const override;
};


