#pragma once
#include "FairyBase.h"

//炎の精霊
class FireFairy : public FairyBase {
public:
	FireFairy();

	//炎の攻撃を開始する
	void StartAttack() override;
};
