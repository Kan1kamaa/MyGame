#pragma once
#include "FairyBase.h"

//風の精霊
class WindFairy : public FairyBase {
public:
	WindFairy();

	//風の攻撃を開始する
	void StartAttack() override;
};
