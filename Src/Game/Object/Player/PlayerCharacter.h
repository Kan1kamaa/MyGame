#pragma once
#include "../ActorBase/ActorBase.h"
#include "../Shot/ShotManager.h"

//プレイヤーが操作する1体分のキャラクターの共通インターフェース
class PlayerCharacter : public ActorBase {

public:
	//攻撃入力を受けて自分固有の攻撃をリクエストする
	virtual void Attack(ShotManager& shotManager) = 0;

	//移動・攻撃入力を受けてアニメーションを切り替える
	//@isAttackTrigger : 攻撃キーが押された瞬間か(トリガー入力。押しっぱなしでは連段させない)
	//@isMoveInput     : 前進キーが押されているか
	//@isRunInput      : ダッシュキーが押されているか
	//@return          : このフレームでコンボの新しい段の攻撃を開始したか(trueならAttack()を呼ぶ)
	virtual bool UpdateAnimState(bool isAttackTrigger, bool isMoveInput, bool isRunInput) = 0;

	//アクティブになった瞬間に待機モーションへ戻す
	virtual void ResetToIdle() = 0;
};
