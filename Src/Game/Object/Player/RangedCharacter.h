#pragma once
#include "PlayerCharacter.h"

//遠距離攻撃キャラクター(キャラ2)
class RangedCharacter : public PlayerCharacter {
public:
	//データロード
	void Load();

	void Attack(ShotManager& shotManager) override;
	void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput) override;
	void ResetToIdle() override;
};
