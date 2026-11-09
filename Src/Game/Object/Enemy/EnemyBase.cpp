#include"EnemyBase.h"

static const int ELEMENT_HOLD_TIME = 900;

EnemyBase::EnemyBase()
{
	m_element = NONE;
	m_elementCnt = 0;
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::Init()
{
	ActorBase::Init();
	m_element = NONE;
	m_elementCnt = 0;
}

void EnemyBase::AddElement(ElementType elementType)
{
	m_element = elementType;
	m_elementCnt = ELEMENT_HOLD_TIME;
}

ElementType EnemyBase::GetElement()const
{
	return m_element;
}

void EnemyBase::StepElement()
{
	if (m_element == NONE)return;
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