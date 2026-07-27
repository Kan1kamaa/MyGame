#include "RangedCharacter.h"
#include "math.h"

namespace {
	const float ANIM_SPEED = 1.0f;	  //アニメーションの速度
	const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離

	//アニメーション一覧
	enum tagAnim2 {
		ANIMID_ATTACK2,		//攻撃モーション
		ANIMID_EX2,
		ANIMID_IDLE2,	   //待機モーション
		ANIMID_RUN2,			//走りモーション
		ANIMID_TPAUSE2,
		ANIMID_WALK2,		//歩きモーション

		ANIMID_NUM2	// 全アニメーション数
	};
}

//データロード
void RangedCharacter::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Player/charcter2/Kan1kama2.mv1");
		RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}

//遠距離攻撃
void RangedCharacter::Attack(ShotManager& shotManager)
{
	VECTOR pos = m_pos;

	//弾の発射場所はプレイヤーの体になるように高さを調整
	pos.y += 10.0f;

	//弾を表示する場所はプレイヤーの回転角度を利用
	pos.x += sinf(m_rot.y) * -ATTACK_DIST;
	pos.z += cosf(m_rot.y) * -ATTACK_DIST;

	//プレイヤーの回転角度を利用して弾の移動速度を計算
	VECTOR speed;
	speed.x = sinf(m_rot.y) * SHOT_SPEED;
	speed.y = 0.0f;
	speed.z = cosf(m_rot.y) * SHOT_SPEED;

	shotManager.RequestPlayerShot(pos, speed);
}

//アニメーション状態の更新
void RangedCharacter::UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput)
{
	switch (m_animData.m_index)
	{
	case ANIMID_IDLE2:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			RequestLoopAnim(isRunInput ? ANIMID_RUN2 : ANIMID_WALK2, ANIM_SPEED);
		}
		break;

	case ANIMID_WALK2:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			if (isRunInput)
			{
				RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
			}
		}
		else
		{
			RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
		}
		break;

	case ANIMID_RUN2:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			if (!isRunInput)
			{
				RequestLoopAnim(ANIMID_WALK2, ANIM_SPEED);
			}
		}
		else
		{
			RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
		}
		break;

	case ANIMID_ATTACK2:
		if (!isAttackInput)
		{
			RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
		}
		break;
	}
}

void RangedCharacter::ResetToIdle()
{
	RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
}
