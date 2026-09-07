#pragma once
#include "PlayerCharacter.h"

//近接攻撃キャラクター(キャラ1)
class MeleeCharacter : public PlayerCharacter {
private:
	int  m_comboStage;      //現在のコンボ段階(0=攻撃していない、1〜3=攻撃中の段)
	bool m_prevAttackInput; //前フレームの攻撃キー入力状態(トリガー判定用)
	bool m_comboQueued;     //次の段への攻撃入力を予約しているか(モーション終盤でつなげる)

	int m_weaponHndl;      //刀のモデルハンドル(左手用)
	int m_weaponHndl2;     //刀のモデルハンドル(右手用。ジャンプ攻撃の両手持ちでのみ使う)
	int m_handFrameIndexL; //左手のボーン(フレーム)番号。見つからなければ-1
	int m_handFrameIndexR; //右手のボーン(フレーム)番号。見つからなければ-1

	//指定した段の攻撃モーションを開始する
	void StartAttack(int stage);
	//ジャンプ開始モーションを再生する
	void StartJump();
	//空中攻撃モーションを再生する。velocityYに上向きの速度を入れて、最後まで空中で出し切れるようにする
	void StartJumpAttack(float& velocityY);
	//スキルモーションを再生する
	void StartSkill();
	//必殺技モーションを再生する
	void StartUlt();
	//攻撃モーション(地上1〜3段目、空中攻撃)を再生中かどうか。刀を持たせるかどうかの判定にも使う
	bool IsAttackMotion() const;

public:
	MeleeCharacter();

	//初期化
	void Init() override;
	//データロード
	void Load();
	//終了処理(刀のモデルの後片付けも行う)
	void Fin();

	void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput,
		bool isGrounded, float& velocityY, bool isJumpTrigger,
		bool isSkillTrigger, bool isUltTrigger) override;
	void ResetToIdle() override;
	bool IsAttacking() const override;
	bool IsAttackActive() const override;
	float GetLungeSpeed() const override;
	void UpdateWeapon() override;
	void DrawWeapon() override;
	int GetWeaponFrameIndex() const override { return m_handFrameIndexL; }
	int GetWeaponFrameIndexR() const override { return m_handFrameIndexR; }
};
