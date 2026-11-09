#include "WindFairy.h"

const float SKILL_HIT_DIST = 50.0f;
const float ANIM_SPEED = 0.5f;
const float PULL_RADIUS = 60.0f;
const float PULL_POWER = 0.5f;
const float SKILL_DAMAGE = 10;
const int HIT_INTERVAL = 20;
//コンストラクタ
WindFairy::WindFairy()
{
	m_SkillCenter = { 0.0f,0.0f,0.0f };
}

//風の攻撃を開始する
void WindFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	if (m_state == SKILL)return;
	m_hitCnt = 0;
	m_state = SKILL;
	RequestAnim(ANIMID_ATTACK, ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, SKILL_HIT_DIST));
	m_effectRequest = EffectManager::EFFECT_WINDSKILL;
}

bool WindFairy::IsSkillActive()const
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

VECTOR WindFairy::GetSkillPos()const
{
	return m_SkillCenter;
}

float WindFairy::GetSkillRadius()const
{
	return PULL_RADIUS;
}

float WindFairy::GetSkillPower()const
{
	return PULL_POWER;
}

float WindFairy::GetAttackPower()const
{
	return SKILL_DAMAGE;
}

bool WindFairy::IsSkillHit()const
{
	if (m_state == SKILL && m_hitCnt > 0 && m_hitCnt % HIT_INTERVAL == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}