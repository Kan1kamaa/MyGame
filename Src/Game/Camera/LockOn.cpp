#include"LockOn.h"

namespace {
	static int LOCK_RANGE = 100;
	static int LOCK_RELEASE_RANGE = 200;
	static int ANGLE_WEIGHT = 10;
	const int BOSS_INDEX = ENEMY_MAX;
}

void LockOn::Init()
{
	m_targetIndex = -1;
	m_targetPos = { 0.0f,0.0f,0.0f };
	m_prevKeyLock = false;
}

void LockOn::Step(EnemyManager& enemy, BossGolem& boss, const VECTOR& playerPos, float cameraYaw)
{
	bool now = false;
	if (CheckHitKey(KEY_INPUT_Q) == true)
	{
		now = true;
	}
	bool trigger = false;
	if (now == true && m_prevKeyLock == false)
	{
		trigger = true;
	}
	m_prevKeyLock = now;
	if (trigger == true)
	{
		if (IsLocked() == true)
		{
			Release();
		}
		else
		{
			m_targetIndex = FindBestTarget(enemy,boss,playerPos,cameraYaw);
		}
	}
}

EnemyBase& LockOn::GetEnemyBase(int index, EnemyManager& enemy, BossGolem& boss)
{
	if (index == BOSS_INDEX)
	{
		return boss;
	}
	return enemy.GetEnemy(index);
}