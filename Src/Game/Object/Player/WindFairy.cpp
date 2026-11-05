#include "WindFairy.h"

const float SKILL_HIT_DIST = 50.0f;
const float ANIM_SPEED = 0.5f;
const float PULL_RADIUS = 60.0f;
const float PULL_POWER = 0.5f;
//コンストラクタ
WindFairy::WindFairy()
{
	m_SkillCenter = { 0.0f,0.0f,0.0f };
}

//風の攻撃を開始する
void WindFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	if (m_state == SKILL)return;
	m_state = SKILL;
	RequestAnim(ANIMID_ATTACK, ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, SKILL_HIT_DIST));
}

bool WindFairy::IsPullActive()const
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

VECTOR WindFairy::GetPullPos()const
{
	return m_SkillCenter;
}

float WindFairy::GetPullRadius()const
{
	return PULL_RADIUS;
}

float WindFairy::GetPullPower()const
{
	return PULL_POWER;
}