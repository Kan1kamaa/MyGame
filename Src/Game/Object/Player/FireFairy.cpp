#include "FireFairy.h"

const float SKILL_HIT_DIST = 50.0f;
const float ANIM_SPEED = 0.5f;

//コンストラクタ
FireFairy::FireFairy()
{
	m_SkillCenter = { 0.0f,0.0f,0.0f };
}

//炎の攻撃を開始する
void FireFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	if (m_state == SKILL)return;
	m_state = SKILL;
	m_hitCnt = 0;
	RequestAnim(ANIMID_ATTACK, ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, SKILL_HIT_DIST));
	//m_effectRequest = EffectManager::EFFECT_WINDSKILL;
}
