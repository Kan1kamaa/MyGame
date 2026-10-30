#include "WindFairy.h"

//コンストラクタ
WindFairy::WindFairy()
{
}

//風の攻撃を開始する
void WindFairy::StartAttack(const VECTOR& playerPos,float playerRotY)
{
	m_state = SKILL;

}
