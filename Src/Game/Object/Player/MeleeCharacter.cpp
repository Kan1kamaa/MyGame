#include "MeleeCharacter.h"
#include "math.h"

namespace {
	const float ANIM_SPEED = 1.0f;	  //アニメーションの速度
	const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離

	//アニメーション一覧
	enum tagAnim {
		ANIMID_TPAUSE,
		ANIMID_ATTACK,		//攻撃モーション
		ANIMID_EX,
		ANIMID_HELLO,
		ANIMID_IDLE,	   //待機モーション
		ANIMID_RUN,			//走りモーション
		ANIMID_WALK,		//歩きモーション

		ANIMID_NUM	// 全アニメーション数
	};
}

//データロード
void MeleeCharacter::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Player/charcter/charcter1.mv1");
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
	}
}

//近接攻撃
void MeleeCharacter::Attack(ShotManager& shotManager)
{
	VECTOR pos = m_pos;

	//弾の発射場所はプレイヤーの体になるように高さを調整
	pos.y += 10.0f;

	//弾を表示する場所はプレイヤーの回転角度を利用
	pos.x += sinf(m_rot.y) * -ATTACK_DIST;
	pos.z += cosf(m_rot.y) * -ATTACK_DIST;

	//近接攻撃は動かす必要がないのでspeedを0にする
	VECTOR speed = { 0.0f,-1.0f,0.0f };

	shotManager.RequestPlayerAttack(pos, speed);
}

//アニメーション状態の更新
void MeleeCharacter::UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput)
{
	switch (m_animData.m_index)
	{
	case ANIMID_IDLE:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			RequestLoopAnim(isRunInput ? ANIMID_RUN : ANIMID_WALK, ANIM_SPEED);
		}
		break;

	case ANIMID_WALK:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			if (isRunInput)
			{
				RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
			}
		}
		else
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_RUN:
		if (isAttackInput)
		{
			RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		}
		else if (isMoveInput)
		{
			if (!isRunInput)
			{
				RequestLoopAnim(ANIMID_WALK, ANIM_SPEED);
			}
		}
		else
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_ATTACK:
		if (!isAttackInput)
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;
	}
}

void MeleeCharacter::ResetToIdle()
{
	RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
}
