#include "ThunderFairy.h"
#include "../../System/ModelUtil.h"
#include "math.h"

ThunderFairy::ThunderFairy()
{
	m_element = FairyElement::Thunder;
}

void ThunderFairy::StartAttack()
{
	//TODO: ChangeState(FairyState::Attack); m_isAttacking = true; にして攻撃を作る
}
