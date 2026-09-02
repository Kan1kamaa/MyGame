#include "RangedCharacter.h"
#include "math.h"

namespace {
	const float ANIM_SPEED = 1.0f;	  //アニメーションの速度
	const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離

	const int COMBO_MAX = 3;			//コンボの最大段数
	const float COMBO_ACCEPT_RATE = 0.5f;	//攻撃モーションの再生がこの割合(0.0〜1.0)を超えたら次段の入力を受け付ける

	//アニメーション一覧
	//※ ATTACK2_1〜3 の並び順・番号は Kan1kama2.mv1 に実際に入っている
	//   攻撃アニメーションの並び順(MV1GetAnimNum/MV1GetAnimNameで確認)に合わせて調整すること
	enum tagAnim2 {
		ANIMID_ATTACK2_1,	//攻撃1段目モーション
		ANIMID_ATTACK2_2,	//攻撃2段目モーション
		ANIMID_ATTACK2_3,	//攻撃3段目モーション
		ANIMID_EX2,
		ANIMID_IDLE2,	   //待機モーション
		ANIMID_RUN2,			//走りモーション
		ANIMID_TPAUSE2,
		ANIMID_WALK2,		//歩きモーション

		ANIMID_NUM2	// 全アニメーション数
	};

	//コンボ段数(1〜3) -> 再生するアニメID
	const int ATTACK_ANIM_TABLE2[COMBO_MAX] = { ANIMID_ATTACK2_1, ANIMID_ATTACK2_2, ANIMID_ATTACK2_3 };
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
//戻り値: このフレームでコンボの新しい段の攻撃を開始したらtrue
bool RangedCharacter::UpdateAnimState(bool isAttackTrigger, bool isMoveInput, bool isRunInput)
{
	//---- 攻撃(コンボ)中 ----
	if (m_comboStep > 0)
	{
		//現在の攻撃モーションがどこまで再生されたか(0.0〜1.0)
		float progress = (m_animData.m_endFrm > 0.0f) ? (m_animData.m_nowFrm / m_animData.m_endFrm) : 1.0f;

		//受付ウィンドウに入ってから攻撃キーが押されたら、次段の入力として予約しておく
		if (isAttackTrigger && progress >= COMBO_ACCEPT_RATE && m_comboStep < COMBO_MAX)
		{
			m_comboReserved = true;
		}

		//現在の攻撃モーションを再生し終えた
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			if (m_comboReserved)
			{
				//次の段の攻撃モーションを開始
				m_comboReserved = false;
				m_comboStep++;
				RequestAnim(ATTACK_ANIM_TABLE2[m_comboStep - 1], ANIM_SPEED);
				return true;
			}

			//追加入力がなかったのでコンボを終了し待機へ戻す
			m_comboStep = 0;
			RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
		}

		return false;
	}

	//---- 攻撃していない状態 ----
	if (isAttackTrigger)
	{
		//コンボ1段目を開始
		m_comboStep = 1;
		m_comboReserved = false;
		RequestAnim(ATTACK_ANIM_TABLE2[0], ANIM_SPEED);
		return true;
	}

	if (isMoveInput)
	{
		RequestLoopAnim(isRunInput ? ANIMID_RUN2 : ANIMID_WALK2, ANIM_SPEED);
	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}

	return false;
}

void RangedCharacter::ResetToIdle()
{
	m_comboStep = 0;
	m_comboReserved = false;
	RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
}
