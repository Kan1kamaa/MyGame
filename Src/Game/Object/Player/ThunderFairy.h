#pragma once
#include "FairyBase.h"

//雷の精霊
class ThunderFairy : public FairyBase {
public:
	ThunderFairy();

	//雷の攻撃を開始する
	void StartAttack(const VECTOR& playerPos,float playerRotY) override;

	bool IsSkillActive()const override;

	VECTOR GetSkillPos()const override;

	float GetSkillRadius()const override;

	float GetAttackPower()const override;

	bool IsSkillHit()const override;
};
