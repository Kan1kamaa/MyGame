#pragma once
#include "../ActorBase/ActorBase.h"
#include "../Shot/ShotManager.h"

//プレイヤーが操作する1体分のキャラクターの共通インターフェース
class PlayerCharacter : public ActorBase {

public:
	//攻撃入力を受けて自分固有の攻撃をリクエストする
	virtual void Attack(ShotManager& shotManager) = 0;

	//移動・攻撃・ジャンプ入力を受けてアニメーションを切り替える
	//@isAttackInput : 攻撃キーが押されているか
	//@isMoveInput   : 前進キーが押されているか
	//@isRunInput    : ダッシュキーが押されているか
	//@isGrounded    : 地面に接地しているか
	//@velocityY     : 現在の上下速度(上昇中はプラス、下降中はマイナス)。参照渡しなので、
	//                 ここで書き換えるとCharacterManager側の実際の上下速度も変わる(空中攻撃の打ち上げなどに使う)
	//@isJumpTrigger  : ジャンプキーが押された瞬間か
	//@isSkillTrigger : スキルキー(E)が押された瞬間か
	//@isUltTrigger   : 必殺技キー(R)が押された瞬間か
	virtual void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput,
		bool isGrounded, float& velocityY, bool isJumpTrigger,
		bool isSkillTrigger, bool isUltTrigger) = 0;

	//アクティブになった瞬間に待機モーションへ戻す
	virtual void ResetToIdle() = 0;

	//攻撃モーション中かどうか(trueの間はCharacterManager側でWASD移動を受け付けない)
	virtual bool IsAttacking() const = 0;

	//攻撃の踏み込みで前進させたい速度(0なら前進しない。攻撃の無いキャラや通常時は0を返す)
	virtual float GetLungeSpeed() const { return 0.0f; }

	//武器モデルの座標を手のボーンに合わせて更新する(武器を持たないキャラは何もしなくてよい)
	virtual void UpdateWeapon() {}

	//武器モデルを描画する(武器を持たないキャラは何もしなくてよい)
	virtual void DrawWeapon() {}

	//デバッグ用:武器を取り付けるボーンのフレーム番号(-1なら見つかっていない/該当キャラでない)
	virtual int GetWeaponFrameIndex() const { return -1; }
	//デバッグ用:右手側(両手持ちする武器がある場合のみ使用)
	virtual int GetWeaponFrameIndexR() const { return -1; }

	//デバッグ用:揺れものボーンのうち、実際に見つかった本数
	virtual int GetJiggleFoundCount() const { return 0; }
	//デバッグ用:揺れものボーンとして登録を試みた本数
	virtual int GetJiggleTotalCount() const { return 0; }
};
