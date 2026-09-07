#pragma once
#include"../Object/Enemy/EnemyManager.h"
#include"../Object/Shot/ShotManager.h"
#include"../Object/Player/CharacterManager.h"
class GameCollision {
public:
	static void CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager);

	static void CheckHitEnemyToPlayer(EnemyManager& enemyManager,CharacterManager& player);

	//プレイヤーの近接攻撃(前方の球)と敵の当たり判定。当たった敵は倒す
	static void CheckHitPlayerAttackToEnemy(CharacterManager& player, EnemyManager& enemyManager);
};