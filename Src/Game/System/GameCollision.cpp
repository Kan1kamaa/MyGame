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

//プレイヤーの近接攻撃と敵の当たり判定
void GameCollision::CheckHitPlayerAttackToEnemy(CharacterManager& player, EnemyManager& enemyManager)
{
	//攻撃モーション中(通常1〜3段・ジャンプ攻撃・スキル・必殺技)でなければ何もしない
	if (player.IsAttackActive() == false)return;

	//攻撃判定の球(キャラの前方に出している)
	VECTOR attackPos = player.GetAttackPos();
	float attackRad = player.GetAttackRadius();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		Enemy& enemy = enemyManager.GetEnemy(i);
		if (enemy.GetActive() == false)continue;

		VECTOR enemyPos = enemy.GetCollisionPos();
		float enemyRad = enemy.GetRadius();

		//球どうしが当たっていれば、その敵を倒す(既存のHitCalcを使う)
		if (Collsion::CheckHitSphereToSphere(attackPos, attackRad, enemyPos, enemyRad) == true)
		{
			enemy.HitCalc(player);
		}
	}
}

//プレイヤーの弾とボスの当たり判定
void GameCollision::CheckHitBossToShot(BossGolem& boss, ShotManager& shotManager)
{
	if (boss.GetActive() == false)return;

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		Shot& plshot = shotManager.GetPlayerShot(i);
		if (plshot.GetActive() == false)continue;

		VECTOR shotPos = plshot.GetCollisionPos();
		float shotRad = plshot.GetRadius();

		if (Collsion::CheckHitSphereToSphere(shotPos, shotRad, bossPos, bossRad) == true)
		{
			plshot.HitCalc(boss);
			boss.HitCalc(plshot);
		}
	}
}

//ボスとプレイヤーの接触判定
void GameCollision::CheckHitBossToPlayer(BossGolem& boss, CharacterManager& player)
{
	if (boss.GetActive() == false)return;

	VECTOR playerPos = player.GetCollisionPos();
	float playerRad = player.GetRadius();

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	if (Collsion::CheckHitSphereToSphere(playerPos, playerRad, bossPos, bossRad) == true)
	{
		player.HitCalc(boss);
	}
}

//プレイヤーの近接攻撃(前方の球)とボスの当たり判定
void GameCollision::CheckHitPlayerAttackToBoss(CharacterManager& player, BossGolem& boss)
{
	//攻撃モーション中(通常1〜3段・ジャンプ攻撃・スキル・必殺技)でなければ何もしない
	if (player.IsAttackActive() == false)return;
	if (boss.GetActive() == false)return;

	VECTOR attackPos = player.GetAttackPos();
	float attackRad = player.GetAttackRadius();

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	if (Collsion::CheckHitSphereToSphere(attackPos, attackRad, bossPos, bossRad) == true)
	{
		boss.HitCalc(player);
	}
}