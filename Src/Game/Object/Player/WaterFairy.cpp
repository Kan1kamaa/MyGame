#include "WaterFairy.h"

//=====================================================
//水のスキル(スキル中、範囲の敵に水を付与し続け、1回だけ小ダメージを与える)
//=====================================================
//--- 位置・範囲 ---
const float WATER_HIT_DIST = 40.0f;		//プレイヤーの前方、水の中心までの距離
const float WATER_RADIUS = 50.0f;		//水を付与する範囲の半径
//--- 強さ ---
const float WATER_POWER = 20.0f;		//ダメージが入るときのダメージ(小ダメージ)
//--- タイミング(フレーム) ---
const int WATER_HIT_FRAME = 65;			//ダメージが入るフレーム(スキル開始からの経過)
const int WATER_SKILL_MINFRAME = 75;	//スキルが続く最低フレーム数
//--- アニメ ---
const float WATER_ANIM_SPEED = 0.5f;	//スキルのアニメの再生速度
//コンストラクタ
WaterFairy::WaterFairy()
{
	m_element = WATER;
}

//水の攻撃を開始する
void WaterFairy::StartAttack(const VECTOR& playerPos, float playerRotY)
{
	if (m_state == SKILL)return;
	m_state = SKILL;
	m_SkillMinFrame = WATER_SKILL_MINFRAME;
	m_hitCnt = 0;
	RequestAnim(ANIMID_ATTACK, WATER_ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, WATER_HIT_DIST));
	m_SkillRotY = playerRotY;
	m_effectRequest = EffectManager::EFFECT_WATERSKILL;
}

bool WaterFairy::IsSkillActive()const
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

VECTOR WaterFairy::GetSkillPos()const
{
	return m_SkillCenter;
}

float WaterFairy::GetSkillRadius()const
{
	return WATER_RADIUS;
}

float WaterFairy::GetAttackPower()const
{
	return WATER_POWER;
}

bool WaterFairy::IsSkillHit()const
{
	if (m_state == SKILL && m_hitCnt == WATER_HIT_FRAME)
	{
		return true;
	}
	else
	{
		return false;
	}
}