#pragma once
#include"../Object/Enemy/EnemyManager.h"
#include"../Object/Enemy/Boss.h"
#include"../Object/Shot/ShotManager.h"
#include"../Object/Player/CharacterManager.h"
#include"../Object/Player/FairyManager.h"
#include"../System/Reaction.h"
#include<vector>
struct ExplosionRequest
{
	VECTOR pos;
	float radius;
	int damage;
};

class GameCollision {
private:
	static std::vector<ExplosionRequest> m_explosionList;

	static void ApplySkillEffect(FairyBase* fairy, EnemyBase& enemy,int k);
	//敵(雑魚・ボス共通)の攻撃判定とプレイヤーの当たり判定
	static void CheckHitAttackToPlayer(EnemyBase& enemy, CharacterManager& player);
public:
	static void CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager);

	//体に触れただけのダメージ(いまは使わない。敵の攻撃は CheckHitEnemyAttackToPlayer を使う)
	static void CheckHitEnemyToPlayer(EnemyManager& enemyManager,CharacterManager& player);

	//敵の攻撃(攻撃モーションの振り下ろしの区間だけ前方の球が当たる)とプレイヤーの当たり判定
	static void CheckHitEnemyAttackToPlayer(EnemyManager& enemyManager, CharacterManager& player);

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

	static void ProcessExplosion(EnemyManager& enemy, BossGolem& boss);
};