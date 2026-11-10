#pragma once
#include "FairyBase.h"

//炎の精霊
class FireFairy : public FairyBase {
private:
	
public:
	FireFairy();

	//炎の攻撃を開始する
	void StartAttack(const VECTOR& playerPos,float playerRotY) override;

	bool IsSkillActive()const override;

	VECTOR GetSkillPos()const override;

	float GetSkillRadius()const override;

	float GetAttackPower()const override;

	bool IsSkillHit()const override;
};
