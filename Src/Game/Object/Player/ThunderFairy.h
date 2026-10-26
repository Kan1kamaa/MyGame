#pragma once
#include "FairyBase.h"

//雷の精霊
class ThunderFairy : public FairyBase {
public:
	ThunderFairy();
	void StartAttack() override;
};
