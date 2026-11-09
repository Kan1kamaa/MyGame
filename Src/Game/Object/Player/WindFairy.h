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

	bool IsSkillActive()const override;

	VECTOR GetSkillPos()const override;

	float GetSkillRadius()const override;

	float GetSkillPower()const override;

	float GetAttackPower()const override;

	bool IsSkillHit()const override;
};


