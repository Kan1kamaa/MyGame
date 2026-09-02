#pragma once
#include "PlayerCharacter.h"

//近接攻撃キャラクター(キャラ1)
class MeleeCharacter : public PlayerCharacter {
private:
	//現在のコンボ段数(0=攻撃していない、1〜3=攻撃1〜3段目を再生中)
	int  m_comboStep = 0;
	//次段の攻撃入力を受け付け済みか(コンボ受付ウィンドウ中に攻撃キーが押された)
	bool m_comboReserved = false;

public:
	//データロード
	void Load();

	void Attack(ShotManager& shotManager) override;
	bool UpdateAnimState(bool isAttackTrigger, bool isMoveInput, bool isRunInput) override;
	void ResetToIdle() override;
};
