#include"EnemyBase.h"

static const int ELEMENT_HOLD_TIME = 900;
static const int DAMAGE_FLASH_TIME = 4;		//TakeDamageで被弾したときに赤く光らせるフレーム数(多段ヒット用に短め)
static const int BURN_DAMAGE = 3;
static const int BURN_INTERVAL = 30;
EnemyBase::EnemyBase()
{
	m_element = NONE;
	m_elementCnt = 0;
	m_flashCnt = 0;
	m_isDying = false;
	m_speed = { 0.0f,0.0f,0.0f };
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

unsigned int EnemyBase::GetElementColor()
{
	return 0;
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

void EnemyBase::ClearElement()
{
	m_element = NONE;
	m_elementCnt = 0;
}