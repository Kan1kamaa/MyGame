#include"EnemyBase.h"

static const int ELEMENT_HOLD_TIME = 900;
static const int DAMAGE_FLASH_TIME = 4;		//TakeDamageで被弾したときに赤く光らせるフレーム数(多段ヒット用に短め)
static const int BURN_DAMAGE = 3;
static const int BURN_INTERVAL = 30;
static const int SHOCK_TIME = 900;
static const int SHOCK_CHECK_INTERVAL = 30;
static const int SHOCK_STUN_CHANCE = 30;
static const int SHOCK_DAMAGE = 2;			//感電中に定期的に受けるダメージ(1回あたり)
static const int SHOCK_DAMAGE_INTERVAL = 60;	//感電中のダメージが入る間隔(フレーム数)

EnemyBase::EnemyBase()
{
	m_element = NONE;
	m_elementCnt = 0;
	m_flashCnt = 0;
	m_isDying = false;
	m_speed = { 0.0f,0.0f,0.0f };
	m_shockCnt = 0;
	m_shockEffectHndl = -1;
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::Init()
{
	ActorBase::Init();
	m_element = NONE;
	m_elementCnt = 0;
	m_flashCnt = 0;
	m_isDying = false;
	m_speed = { 0.0f,0.0f,0.0f };
	m_shockCnt = 0;
	m_shockEffectHndl = -1;
}

void EnemyBase::AddElement(ElementType elementType)
{
	if (elementType == WIND || elementType == NONE)return;
	m_element = elementType;
	m_elementCnt = ELEMENT_HOLD_TIME;
}

ElementType EnemyBase::GetElement()const
{
	return m_element;
}

void EnemyBase::StepBase()
{
	StepElement();
	if (m_flashCnt > 0)
	{
		m_flashCnt--;
	}
	if (m_shockCnt > 0)
	{
		//感電中は、一定間隔でダメージを受ける(硬直とは別に入る)
		if (m_shockCnt % SHOCK_DAMAGE_INTERVAL == 0)
		{
			TakeDamage(SHOCK_DAMAGE);
		}
		m_shockCnt--;
	}
}

void EnemyBase::StepElement()
{
	if (m_element == NONE)return;
	
	switch (m_element)
	{
	case NONE:
		break;
	case WIND:
		break;
	case FIRE:
		if (m_elementCnt % BURN_INTERVAL == 0)
		{
			TakeDamage(BURN_DAMAGE);
		}
		break;
	case WATER:
		break;
	case THUNDER:
		break;
	}

	m_elementCnt--;
	if (m_elementCnt <= 0)
	{
		m_element = NONE;
	}
}

COLOR_F EnemyBase::GetElementColor()const
{
	switch (m_element)
	{
	case NONE:
		return{ 1.0f,1.0f,1.0f,1.0f };
	case WIND:
		return{ 1.0f,1.0f,1.0f,1.0f };
	case FIRE:
		return{ 1.0f,0.5f,0.2f,1.0f };
	case WATER:
		return{ 0.3f,0.5f,1.0f,1.0f };
	case THUNDER:
		return{ 1.0f,1.0f,0.5f,1.0f };
	}
	return{ 1.0f,1.0f,1.0f,1.0f };
}

void EnemyBase::Pull(const VECTOR& center, float power)
{

}

void EnemyBase::TakeDamage(int damage)
{
	if (m_isDying == true)return;
	m_status.AddDamage(damage);
	m_flashCnt = DAMAGE_FLASH_TIME;
	if (m_status.NowHp <= 0)
	{
		OnDead();
		m_isDying = true;
		m_speed = { 0.0f,0.0f,0.0f };
	}
}

void EnemyBase::OnDead()
{

}

bool EnemyBase::IsDying()const
{
	return m_isDying;
}

void EnemyBase::ClearElement()
{
	m_element = NONE;
	m_elementCnt = 0;
}

void EnemyBase::StartShock()
{
	m_shockCnt = SHOCK_TIME;
}

bool EnemyBase::IsShocked()const
{
	if (m_shockCnt > 0)
	{
		return true;
	}
	return false;
}

bool EnemyBase::IsStunned()const
{
	return false;
}

bool EnemyBase::RollShockStun()const
{
	if (IsShocked() == false)return false;
	if (m_shockCnt % SHOCK_CHECK_INTERVAL != 0)
	{
		return false;
	}
	if (GetRand(99) < SHOCK_STUN_CHANCE)
	{
		return true;
	}
	return false;

}

int EnemyBase::GetShockEffectHndl()const
{
	return m_shockEffectHndl;
}

void EnemyBase::SetShockEffectHndl(int hndl)
{
	m_shockEffectHndl = hndl;
}