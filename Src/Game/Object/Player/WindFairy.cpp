#include "WindFairy.h"

//=====================================================
//風のスキル(敵を中心へ引き寄せつつ、弱いダメージを何度も与える)
//=====================================================
//--- 位置・範囲 ---
const float WIND_HIT_DIST = 40.0f;		//プレイヤーの前方、渦の中心までの距離
const float WIND_PULL_RADIUS = 50.0f;	//引き寄せる範囲の半径
//--- 強さ ---
const float WIND_PULL_POWER = 0.5f;		//引き寄せる強さ(1フレームに中心へ近づく量)
const float WIND_DAMAGE = 5.0f;			//1ヒットあたりのダメージ
//--- タイミング(フレーム) ---
const int WIND_HIT_INTERVAL = 20;		//ダメージが入る間隔
const int WIND_SKILL_MINFRAME = 120;	//スキルが続く最低フレーム数
//--- アニメ ---
const float WIND_ANIM_SPEED = 0.5f;		//スキルのアニメの再生速度
//コンストラクタ
WindFairy::WindFairy()
{
	m_element = WIND;
}

//風の攻撃を開始する
void WindFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	if (m_state == SKILL)return;
	m_hitCnt = 0;
	m_state = SKILL;
	m_SkillMinFrame = WIND_SKILL_MINFRAME;
	RequestAnim(ANIMID_ATTACK, WIND_ANIM_SPEED);
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
	VECTOR forward = { -sinf(playerRotY), 0.0f, -cosf(playerRotY) };
	m_SkillCenter = VAdd(playerPos, VScale(forward, WIND_HIT_DIST));
	m_SkillRotY = playerRotY;
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
	return WIND_PULL_RADIUS;
}

float WindFairy::GetSkillPower()const
{
	return WIND_PULL_POWER;
}

float WindFairy::GetAttackPower()const
{
	return WIND_DAMAGE;
}

bool WindFairy::IsSkillHit()const
{
	if (m_state == SKILL && m_hitCnt > 0 && m_hitCnt % WIND_HIT_INTERVAL == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}