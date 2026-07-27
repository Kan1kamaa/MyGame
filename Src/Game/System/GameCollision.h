#pragma once
#include"../Object/Enemy/EnemyManager.h"
#include"../Object/Shot/ShotManager.h"
#include"../Object/Player/CharacterManager.h"
class GameCollision {
public:
	static void CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager);

	static void CheckHitEnemyToPlayer(EnemyManager& enemyManager,CharacterManager& player);
};