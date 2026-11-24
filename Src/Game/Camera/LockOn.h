#pragma once
#include"../../Game/Object/Enemy/EnemyManager.h"
#include"../Object/Enemy/Boss.h"
class LockOn
{
public:
	

	void Init();

	void Step(EnemyManager& enemy,BossGolem& boss,const VECTOR& playerPos,float cameraYaw);

	void Draw();

	bool IsLocked()const;

	VECTOR GetTargetPos()const;

private:

	int m_targetIndex;
	VECTOR m_targetPos;
	bool m_prevKeyLock;

	int FindBestTarget(EnemyManager& enemy, BossGolem& boss, const VECTOR& playerPos, float cameraYaw);

	bool IsTargetable(int index, EnemyManager& enemy, BossGolem& boss);

	VECTOR GetTargetCenter(int index, EnemyManager& enemy, BossGolem& boss);

	void Release();

	EnemyBase& GetEnemyBase(int index, EnemyManager& enemy, BossGolem& boss);
};