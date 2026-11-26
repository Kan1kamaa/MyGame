#include"LockOn.h"
#include"math.h"
namespace {
	static int LOCK_RANGE = 200;
	static int LOCK_RELEASE_RANGE = 300;
	static int ANGLE_WEIGHT = 300;
	const float LOCK_MAX_ANGLE = DX_PI_F / 3;
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
	if (IsLocked() == false)
	{
		return;
	}
	if (IsTargetable(m_targetIndex, enemy, boss) == false)
	{
		Release();
		return;
	}
	VECTOR targetCenter = GetTargetCenter(m_targetIndex, enemy, boss);
	VECTOR toTarget = VSub(targetCenter, playerPos);
	toTarget.y = 0.0f;
	float dist = VSize(toTarget);
	if (dist > LOCK_RELEASE_RANGE)
	{
		Release();
		return;
	}
	m_targetPos = targetCenter;
}

bool LockOn::IsTargetable(int index, EnemyManager& enemy, BossGolem& boss)
{
	if (index > BOSS_INDEX || index < 0)
	{
		return false;
	}
	EnemyBase& target = GetEnemyBase(index, enemy, boss);
	if (target.GetActive() == false)
	{
		return false;
	}
	if (target.IsDying() == true)
	{
		return false;
	}
	return true;
}

VECTOR LockOn::GetTargetCenter(int index, EnemyManager& enemy, BossGolem& boss)
{
	EnemyBase& target = GetEnemyBase(index, enemy, boss);
	return target.GetCollisionPos();
}

int LockOn::FindBestTarget(EnemyManager& enemy, BossGolem& boss, const VECTOR& playerPos, float cameraYaw)
{
	int bestIndex = -1;
	float bestscore = 100000.0f;
	for (int i = 0; i <= BOSS_INDEX; i++)
	{
		if (IsTargetable(i,enemy,boss) == false)
		{
			continue;
		}
		EnemyBase& target = GetEnemyBase(i, enemy, boss);
		VECTOR toEnemy = VSub(target.GetPos(),playerPos);
		toEnemy.y = 0.0f;
		float dist = VSize(toEnemy);
		if (dist > LOCK_RANGE)
		{
			continue;
		}
		float enemyAngle = atan2f(-toEnemy.x, -toEnemy.z);
		float angleDiff = enemyAngle - cameraYaw;
		while (angleDiff > DX_PI_F)
		{
			angleDiff -= DX_PI_F * 2.0f;
		}
		while (angleDiff < -DX_PI_F)
		{
			angleDiff += DX_PI_F * 2.0f;
		}
		angleDiff = fabsf(angleDiff);
		if (angleDiff > LOCK_MAX_ANGLE)
		{
			continue;
		}
		float score = dist + angleDiff * ANGLE_WEIGHT;
		if (score < bestscore)
		{
			bestIndex = i;
			bestscore = score;
		}
	}
	return bestIndex;
}

EnemyBase& LockOn::GetEnemyBase(int index, EnemyManager& enemy, BossGolem& boss)
{
	if (index == BOSS_INDEX)
	{
		return boss;
	}
	return enemy.GetEnemy(index);
}

VECTOR LockOn::GetTargetPos()const
{
	return m_targetPos;
}

bool LockOn::IsLocked()const
{
	if (m_targetIndex != -1)
	{
		return true;
	}
	return false;
}

void LockOn::Release()

{
	m_targetIndex = -1;
}

void LockOn::Draw()
{
	if (IsLocked() == false)
	{
		return;
	}
	VECTOR screenPos = ConvWorldPosToScreenPos(m_targetPos);
	if (screenPos.z > 1 || screenPos.z < 0)
	{
		return;
	}
	DrawCircle((int)screenPos.x, (int)screenPos.y,15,GetColor(255,60,60), false);
}