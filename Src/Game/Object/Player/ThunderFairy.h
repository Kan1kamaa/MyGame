#pragma once
#include "FairyBase.h"

//雷の精霊
class ThunderFairy : public FairyBase {
public:
	ThunderFairy();

	//雷の攻撃を開始する
	void StartAttack(const VECTOR& playerPos,float playerRotY) override;
};
