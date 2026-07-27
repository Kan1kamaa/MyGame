#pragma once
#include "../ActorBase/ActorBase.h"
#include "../Shot/ShotManager.h"

//プレイヤーが操作する1体分のキャラクターの共通インターフェース
class PlayerCharacter : public ActorBase {

public:
	//攻撃入力を受けて自分固有の攻撃をリクエストする
	virtual void Attack(ShotManager& shotManager) = 0;

	//移動・攻撃入力を受けてアニメーションを切り替える
	//@isAttackInput : 攻撃キーが押されているか
	//@isMoveInput   : 前進キーが押されているか
	//@isRunInput    : ダッシュキーが押されているか
	virtual void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput) = 0;

	//アクティブになった瞬間に待機モーションへ戻す
	virtual void ResetToIdle() = 0;
};
