#pragma once
#include"../Object/Enemy/EnemyManager.h"
#include"../Object/Enemy/Boss.h"
#include"../Object/Shot/ShotManager.h"
#include"../Object/Player/CharacterManager.h"
#include"../Object/Player/FairyManager.h"
class GameCollision {
private:
	static void ApplySkillEffect(FairyBase* fairy, EnemyBase& enemy,int k);
public:
	static void CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager);

	static void CheckHitEnemyToPlayer(EnemyManager& enemyManager,CharacterManager& player);

	//プレイヤーの近接攻撃(前方の球)と敵の当たり判定。
	static void CheckHitPlayerAttackToEnemy(CharacterManager& player, EnemyManager& enemyManager);

	//プレイヤーの弾とボスの当たり判定
	static void CheckHitBossToShot(BossGolem& boss, ShotManager& shotManager);

	//ボスの攻撃とプレイヤーの当たり判定
	static void CheckHitBossAttackToPlayer(BossGolem& boss, CharacterManager& player);

	//プレイヤーの近接攻撃(前方の球)とボスの当たり判定
	static void CheckHitPlayerAttackToBoss(CharacterManager& player, BossGolem& boss);

	//精霊のスキルと敵の当たり判定
	static void CheckHitFairySkillToEnemy(FairyManager& fairyManager, EnemyManager& enemyManager);

	//精霊のスキルとボスの当たり判定(ボスは引き寄せず、ダメージだけ与える)
	static void CheckHitFairySkillToBoss(FairyManager& fairyManager, BossGolem& boss);
};