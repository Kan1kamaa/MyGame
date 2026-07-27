#pragma once
#include "PlayerCharacter.h"

//近接攻撃キャラクター(キャラ1)
class MeleeCharacter : public PlayerCharacter {
public:
	//データロード
	void Load();

	void Attack(ShotManager& shotManager) override;
	void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput) override;
	void ResetToIdle() override;
};
