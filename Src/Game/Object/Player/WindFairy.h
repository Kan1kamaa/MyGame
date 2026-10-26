#pragma once
#include "FairyBase.h"

//風の精霊
class WindFairy : public FairyBase {
public:
	WindFairy();
	void StartAttack() override;
};
