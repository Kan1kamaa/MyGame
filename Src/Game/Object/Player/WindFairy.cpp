#include "WindFairy.h"
#include "../../System/ModelUtil.h"
#include "math.h"

WindFairy::WindFairy()
{
	m_element = FairyElement::Wind;
}

void WindFairy::StartAttack()
{
	//TODO: ChangeState(FairyState::Attack); m_isAttacking = true; にして攻撃を作る
}
