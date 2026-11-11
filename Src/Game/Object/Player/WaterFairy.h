#pragma once
#include "FairyBase.h"

//水の精霊
class WaterFairy : public FairyBase {
private:

public:
	WaterFairy();

	//水の攻撃を開始する
	void StartAttack(const VECTOR& playerPos, float playerRotY) override;

	bool IsSkillActive()const override;

	VECTOR GetSkillPos()const override;

	float GetSkillRadius()const override;

	float GetAttackPower()const override;

	bool IsSkillHit()const override;
};
