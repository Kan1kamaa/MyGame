#include "ThunderFairy.h"

const float THUNDER_RADIUS = 80.0f;
const float THUNDER_POWER = 30.0f;
const float THUNDER_HIT_FRAME = 50.0f;
const float ANIM_SPEED = 0.5f;
const float THUNDER_HIT_DIST = 15;
const int THUNDER_SKILL_MINFRAME = 100;
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
	RequestAnim(ANIMID_ATTACK, ANIM_SPEED);
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
	if (m_state == SKILL && m_hitCnt == THUNDER_HIT_FRAME)
	{
		return true;
	}
	else
	{
		return false;
	}
}