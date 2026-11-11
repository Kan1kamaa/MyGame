#include "FireFairy.h"

//=====================================================
//炎のスキル(前方に大ダメージを1回与え、炎を付与して残り火で持続ダメージ)
//=====================================================
//--- 位置・範囲 ---
const float FIRE_HIT_DIST = 40.0f;		//プレイヤーの前方、炎の中心までの距離
const float FIRE_RADIUS = 50.0f;		//当たり判定の半径
//--- 強さ ---
const float FIRE_POWER = 100.0f;		//初撃のダメージ
//--- タイミング(フレーム) ---
const int FIRE_HIT_FRAME = 120;			//初撃が当たるフレーム(スキル開始からの経過)
const int FIRE_SKILL_MINFRAME = 130;	//スキルが続く最低フレーム数
//--- アニメ ---
const float FIRE_ANIM_SPEED = 0.5f;		//スキルのアニメの再生速度
//コンストラクタ
FireFairy::FireFairy()
{
	m_element = FIRE;
}

//炎の攻撃を開始する
void FireFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	if (m_state == SKILL)return;
	m_state = SKILL;
	m_SkillMinFrame = FIRE_SKILL_MINFRAME;
	m_hitCnt = 0;
	RequestAnim(ANIMID_ATTACK, FIRE_ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, FIRE_HIT_DIST));
	m_SkillRotY = playerRotY;
	m_effectRequest = EffectManager::EFFECT_FIRESKILL;
}

bool FireFairy::IsSkillActive()const
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

VECTOR FireFairy::GetSkillPos()const
{
	return m_SkillCenter;
}

float FireFairy::GetSkillRadius()const
{
	return FIRE_RADIUS;
}

float FireFairy::GetAttackPower()const
{
	return FIRE_POWER;
}

bool FireFairy::IsSkillHit()const
{
	if (m_state == SKILL && m_hitCnt == FIRE_HIT_FRAME)
	{
		return true;
	}
	else
	{
		return false;
	}
}