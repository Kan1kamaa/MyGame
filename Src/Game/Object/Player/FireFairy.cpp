#include "FireFairy.h"
#include "../../System/ModelUtil.h"
#include "math.h"

FireFairy::FireFairy()
{
	m_element = FairyElement::Fire;
}

void FireFairy::StartAttack()
{
	//TODO: ChangeState(FairyState::Attack); m_isAttacking = true; にして攻撃を作る
}
