#include"GameCollision.h"
#include"../../Lib/Collision/Collsion.h"

void GameCollision::CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager)
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		Shot& plshot = shotManager.GetPlayerShot(i);
		if (plshot.GetActive() == false)continue;


		VECTOR shotPos = plshot.GetCollisionPos();
		float shotRad = plshot.GetRadius();

		for (int j = 0; j < ENEMY_MAX; j++)
		{
			Enemy& enemy = enemyManager.GetEnemy(j);
			if (enemy.GetActive() == false)continue;

			VECTOR enemyPos = enemy.GetCollisionPos();
			float enemyRad = enemy.GetRadius();

			if (Collsion::CheckHitSphereToSphere(shotPos, shotRad, enemyPos, enemyRad) == true)
			{
				plshot.HitCalc(enemy);
				enemy.HitCalc(plshot);
			}
		}
	}
}

void GameCollision::CheckHitEnemyToPlayer(EnemyManager& enemyManager,CharacterManager& player)
{
	
	VECTOR playerPos = player.GetCollisionPos();
	float playerRad = player.GetRadius();
	for (int j = 0; j < ENEMY_MAX; j++)
	{
		Enemy& enemy = enemyManager.GetEnemy(j);
		if (enemy.GetActive() == false)continue;

		VECTOR enemyPos = enemy.GetCollisionPos();
		float enemyRad = enemy.GetRadius();

		if (Collsion::CheckHitSphereToSphere(playerPos, playerRad, enemyPos, enemyRad) == true)
		{
			player.HitCalc(enemy);
		}
	}
}