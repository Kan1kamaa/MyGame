#include "ThunderFairy.h"

//=====================================================
//雷のスキル(最初の1本が3方向に分かれて伸びていく。1本ずつに判定があり、大きい敵は複数ヒットする)
//球の番号: 0=最初の1本、1〜(方向の数×1方向の本数)=分かれた雷
//=====================================================
//--- 構成 ---
const int THUNDER_BRANCH_NUM = 3;			//分かれる方向の数
const int THUNDER_BRANCH_BOLT_NUM = 4;		//1方向に伸びる雷の本数
const int THUNDER_HIT_NUM = 1 + THUNDER_BRANCH_BOLT_NUM * THUNDER_BRANCH_NUM;	//判定の球の総数(最初の1本+分かれた雷)
//--- 位置・範囲 ---
const float THUNDER_HIT_DIST = 20.0f;		//プレイヤーの前方、最初の雷までの距離
const float THUNDER_STEP_DIST = 17.0f;		//雷どうしの間隔
const float THUNDER_BRANCH_ANGLE = 35.0f;	//分かれる方向が正面から開く角度(度)
const float THUNDER_RADIUS = 10.0f;			//雷1本の判定の半径
//--- 強さ ---
const float THUNDER_POWER = 15.0f;			//雷1本あたりのダメージ
//--- タイミング(フレーム) ---
const int THUNDER_FIRST_HIT_FRAME = 20;		//最初の1本が当たるフレーム(スキル開始からの経過)
const int THUNDER_BRANCH_START_FRAME = 40;	//分かれた雷が伸び始めるフレーム
const int THUNDER_STEP_DELAY = 20;			//外側の雷ほど遅れて当たる、1本ごとの遅れ
const int THUNDER_SKILL_MINFRAME = 110;		//スキルが続く最低フレーム数
//--- アニメ ---
const float THUNDER_ANIM_SPEED = 0.5f;		//スキルのアニメの再生速度
//コンストラクタ
ThunderFairy::ThunderFairy()
{
	m_element = THUNDER;
}

//雷の攻撃を開始する
void ThunderFairy::StartAttack(const VECTOR& playerPos, float playerRotY)
{
	if (m_state == SKILL)return;
	m_state = SKILL;
	m_SkillMinFrame = THUNDER_SKILL_MINFRAME;
	m_hitCnt = 0;
	RequestAnim(ANIMID_ATTACK, THUNDER_ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, THUNDER_HIT_DIST));
	m_SkillRotY = playerRotY;
	m_effectRequest = EffectManager::EFFECT_THUNDERSKILL;
}

bool ThunderFairy::IsSkillActive()const
{
	if (m_state == SKILL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

VECTOR ThunderFairy::GetSkillPos()const
{
	return m_SkillCenter;
}

float ThunderFairy::GetSkillRadius()const
{
	return THUNDER_RADIUS;
}

float ThunderFairy::GetAttackPower()const
{
	return THUNDER_POWER;
}

bool ThunderFairy::IsSkillHit()const
{
	if (m_state == SKILL && m_hitCnt == THUNDER_FIRST_HIT_FRAME)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int ThunderFairy::GetSkillHitNum()const
{
	return THUNDER_HIT_NUM;
}

VECTOR ThunderFairy::GetSkillHitPos(int index)const
{
	if (index == 0)
	{
		return m_SkillCenter;
	}
	else
	{
		int branchIdx = (index - 1) / THUNDER_BRANCH_BOLT_NUM;
		int boltIdx = (index-1) % THUNDER_BRANCH_BOLT_NUM;
		float angledeg = (branchIdx - 1) * THUNDER_BRANCH_ANGLE;
		float dirRotY = angledeg* (DX_PI_F/180.0f);
		dirRotY += m_SkillRotY;
		VECTOR dir = { -sinf(dirRotY),0.0f,-cosf(dirRotY) };
		VECTOR ThunderPos = VAdd(m_SkillCenter, VScale(dir, THUNDER_STEP_DIST * (boltIdx + 1)));
		return ThunderPos;
	}
}

float ThunderFairy::GetSkillHitRadius(int index)const
{
	return THUNDER_RADIUS;
}

bool ThunderFairy::IsSkillHitAt(int index)const
{
	if (m_state != SKILL)
	{
		return false;
	}
	if (index == 0 && m_hitCnt == THUNDER_FIRST_HIT_FRAME)
	{
		return true;
	}
	if (index >= 1)
	{
		int boltidx = (index - 1) % THUNDER_BRANCH_BOLT_NUM;
		int hitFrame = THUNDER_BRANCH_START_FRAME + boltidx * THUNDER_STEP_DELAY;
		if (m_hitCnt == hitFrame)
		{
			return true;
		}
	}
	return false;
}