#pragma once
#include "FairyBase.h"

//炎の精霊
class FireFairy : public FairyBase {
private:
	VECTOR m_SkillCenter;
public:
	FireFairy();

	//炎の攻撃を開始する
	void StartAttack(const VECTOR& playerPos,float playerRotY) override;
};
