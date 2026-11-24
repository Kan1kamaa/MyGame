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

//爆発の範囲のデバッグ表示用(処理した爆発を、数フレームだけ残して表示する)
struct ExplosionDebugInfo
{
	VECTOR pos;
	float radius;
	int cnt;	//表示を続ける残りフレーム数
};

class GameCollision {
private:
	static std::vector<ExplosionRequest> m_explosionList;
	static std::vector<ExplosionDebugInfo> m_explosionDebugList;

	static void ApplySkillEffect(FairyBase* fairy, EnemyBase& enemy,int k, EffectManager& effect);
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
	static void CheckHitFairySkillToEnemy(FairyManager& fairyManager, EnemyManager& enemyManager, EffectManager& effect);

	//精霊のスキルとボスの当たり判定(ボスは引き寄せず、ダメージだけ与える)
	static void CheckHitFairySkillToBoss(FairyManager& fairyManager, BossGolem& boss, EffectManager& effect);

	//記録された爆発を処理する(周囲の敵とボスにダメージ)。スキルの判定のあとに1フレームに1回呼ぶ
	static void ProcessExplosion(EnemyManager& enemyManager, BossGolem& boss,EffectManager& effect);

	//爆発の範囲を、ワイヤーフレームで表示する(デバッグ用)
	static void DrawExplosionDebug();

	static void UpdateShockEffect(EnemyBase& enemy, EffectManager& effect);

	static void ProcessShockEffectManager(EnemyManager& enemyManager, BossGolem& boss, EffectManager& effect);
};