#include"GameCollision.h"
#include"../../Lib/Collision/Collsion.h"


static const int OVERLOAD_RADIUS = 50;
static const int OVERLOAD_DAMAGE = 30;

static const int EXPLOSION_DEBUG_TIME = 30;	//爆発の範囲を表示し続けるフレーム数(デバッグ用)

std::vector<ExplosionRequest>GameCollision::m_explosionList;
std::vector<ExplosionDebugInfo>GameCollision::m_explosionDebugList;

void GameCollision::ApplySkillEffect(FairyBase* fairy, EnemyBase& enemy,int k)
{
	ElementType element = fairy->GetFairyElement();
	ReactionResult result = calcReaction(enemy.GetElement(), element);
	
	if (element == NONE)
	{
		return;
	}
	if (element == WIND)
	{
		enemy.Pull(fairy->GetSkillHitPos(k),fairy->GetSkillPower());
		if (fairy->IsSkillHitAt(k) == true)
		{
			enemy.TakeDamage((int)fairy->GetAttackPower());
		}
		return;
	}
	if (fairy->IsSkillHitAt(k) == false)
	{
		return;
	}
	int damage = (int)(fairy->GetAttackPower() * result.damageRate);
	enemy.TakeDamage(damage);
	if (result.startShock == true)
	{
		enemy.StartShock();
	}
	if (result.type == REACTION_OVERLOAD)
	{
		ExplosionRequest request;
		request.pos = enemy.GetCollisionPos();
		request.radius = OVERLOAD_RADIUS;
		request.damage = OVERLOAD_DAMAGE;
		m_explosionList.push_back(request);
		enemy.ClearElement();
	}
	else
	{
		enemy.AddElement(element);
	}
}

void GameCollision::ProcessExplosion(EnemyManager& enemyManager, BossGolem& boss,EffectManager& effect)
{
	//デバッグ表示用の爆発を、表示時間が過ぎたものから消す
	for (int i = 0; i < m_explosionDebugList.size();)
	{
		m_explosionDebugList[i].cnt--;
		if (m_explosionDebugList[i].cnt <= 0)
		{
			m_explosionDebugList.erase(m_explosionDebugList.begin() + i);
		}
		else
		{
			i++;
		}
	}

	for (int i = 0; i < m_explosionList.size(); i++)
	{
		VECTOR explosionPos = m_explosionList[i].pos;
		float explosionRadius = m_explosionList[i].radius;
		int explosionDamage = m_explosionList[i].damage;
		effect.Play(EffectManager::EFFECT_OVERLOAD, explosionPos, 0.0f);
		//処理した爆発を、デバッグ表示用に残す
		ExplosionDebugInfo debugInfo;
		debugInfo.pos = explosionPos;
		debugInfo.radius = explosionRadius;
		debugInfo.cnt = EXPLOSION_DEBUG_TIME;
		m_explosionDebugList.push_back(debugInfo);

		for (int j = 0; j < ENEMY_MAX; j++)
		{
			Enemy& enemy = enemyManager.GetEnemy(j);
			if (enemy.GetActive() == false)
			{
				continue;
			}
			VECTOR enemyPos = enemy.GetCollisionPos();
			float enemyRadius = enemy.GetRadius();
			if (Collsion::CheckHitSphereToSphere(explosionPos, explosionRadius, enemyPos, enemyRadius) == true)
			{
				enemy.TakeDamage(explosionDamage);
			}
		}
		if (boss.GetActive() == false)
		{
			continue;
		}
		VECTOR bossPos = boss.GetCollisionPos();
		float bossRadius = boss.GetRadius();
		if (Collsion::CheckHitSphereToSphere(explosionPos, explosionRadius, bossPos, bossRadius) == true)
		{
			boss.TakeDamage(explosionDamage);
		}
	}
	m_explosionList.clear();
}

//爆発の範囲を、ワイヤーフレームで表示する(デバッグ用)
void GameCollision::DrawExplosionDebug()
{
	for (int i = 0; i < m_explosionDebugList.size(); i++)
	{
		DrawSphere3D(m_explosionDebugList[i].pos, m_explosionDebugList[i].radius, 24,
			GetColor(255, 160, 0), GetColor(0, 0, 0), FALSE);
	}
}

void GameCollision::CheckHitEnemyToShot(EnemyManager& enemyManager, ShotManager& shotManager)
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		Shot& plshot = shotManager.GetPlayerShot(i);
		if (plshot.GetActive() == false)
		{
			continue;
		}


		VECTOR shotPos = plshot.GetCollisionPos();
		float shotRad = plshot.GetRadius();

		for (int j = 0; j < ENEMY_MAX; j++)
		{
			Enemy& enemy = enemyManager.GetEnemy(j);
			if (enemy.GetActive() == false)
			{
				continue;
			}

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
		if (enemy.GetActive() == false)
		{
			continue;
		}

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
	if (player.IsAttackActive() == false)
	{
		return;
	}

	//攻撃判定の球(キャラの前方に出している)
	VECTOR attackPos = player.GetAttackPos();
	float attackRad = player.GetAttackRadius();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		Enemy& enemy = enemyManager.GetEnemy(i);
		if (enemy.GetActive() == false)
		{
			continue;
		}

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
	if (boss.GetActive() == false)
	{
		return;
	}

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		Shot& plshot = shotManager.GetPlayerShot(i);
		if (plshot.GetActive() == false)
		{
			continue;
		}

		VECTOR shotPos = plshot.GetCollisionPos();
		float shotRad = plshot.GetRadius();

		if (Collsion::CheckHitSphereToSphere(shotPos, shotRad, bossPos, bossRad) == true)
		{
			plshot.HitCalc(boss);
			boss.HitCalc(plshot);
		}
	}
}

//ボスの攻撃とプレイヤーの当たり判定(体に触れただけではダメージを受けない)
void GameCollision::CheckHitBossAttackToPlayer(BossGolem& boss, CharacterManager& player)
{
	CheckHitAttackToPlayer(boss, player);
}

//敵の攻撃(攻撃モーションの振り下ろしの区間だけ前方の球が当たる)とプレイヤーの当たり判定
void GameCollision::CheckHitEnemyAttackToPlayer(EnemyManager& enemyManager, CharacterManager& player)
{
	for (int j = 0; j < ENEMY_MAX; j++)
	{
		CheckHitAttackToPlayer(enemyManager.GetEnemy(j), player);
	}
}

//敵(雑魚・ボス共通)の攻撃判定とプレイヤーの当たり判定
void GameCollision::CheckHitAttackToPlayer(EnemyBase& enemy, CharacterManager& player)
{
	//攻撃判定を出しているとき(振り下ろしの区間など)でなければ何もしない
	if (enemy.GetActive() == false)
	{
		return;
	}
	if (enemy.IsAttackActive() == false)
	{
		return;
	}

	VECTOR playerPos = player.GetCollisionPos();
	float playerRad = player.GetRadius();

	VECTOR attackPos = enemy.GetAttackPos();
	float attackRad = enemy.GetAttackRadius();

	if (Collsion::CheckHitSphereToSphere(playerPos, playerRad, attackPos, attackRad) == true)
	{
		player.HitCalc(enemy);
	}
}

//プレイヤーの近接攻撃(前方の球)とボスの当たり判定
void GameCollision::CheckHitPlayerAttackToBoss(CharacterManager& player, BossGolem& boss)
{
	//攻撃モーション中(通常1〜3段・ジャンプ攻撃・スキル・必殺技)でなければ何もしない
	if (player.IsAttackActive() == false)
	{
		return;
	}
	if (boss.GetActive() == false)
	{
		return;
	}

	VECTOR attackPos = player.GetAttackPos();
	float attackRad = player.GetAttackRadius();

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	if (Collsion::CheckHitSphereToSphere(attackPos, attackRad, bossPos, bossRad) == true)
	{
		boss.HitCalc(player);
	}
}

//精霊のスキルとボスの当たり判定(ボスは引き寄せず、ダメージだけ与える)
void GameCollision::CheckHitFairySkillToBoss(FairyManager& fairyManager, BossGolem& boss)
{
	//ボスが出現していなければ何もしない
	if (boss.GetActive() == false)
	{
		return;
	}

	VECTOR bossPos = boss.GetCollisionPos();
	float bossRad = boss.GetRadius();

	for (int i = 0; i < fairyManager.GetFairyCount(); i++)
	{
		//いない精霊は無視
		FairyBase* fairy = fairyManager.GetFairy(i);
		if (fairy == nullptr)
		{
			continue;
		}
		//スキル中の精霊だけ処理する
		if (fairy->IsSkillActive() == false)
		{
			continue;
		}
		for (int k = 0; k < fairy->GetSkillHitNum(); k++)
		{
			if (Collsion::CheckHitSphereToSphere(fairy->GetSkillHitPos(k), fairy->GetSkillHitRadius(k), bossPos, bossRad) == true)
			{
				ApplySkillEffect(fairy, boss,k);
			}
		}
	}
}

//精霊のスキルと敵の当たり判定
void GameCollision::CheckHitFairySkillToEnemy(FairyManager& fairyManager,EnemyManager& enemyManager)
{
	for (int i = 0; i < fairyManager.GetFairyCount(); i++)
	{
		//いない精霊は無視
		FairyBase* fairy = fairyManager.GetFairy(i);
		if (fairy == nullptr)
		{
			continue;
		}
		//スキル中の精霊だけ処理する
		if (fairy->IsSkillActive() == false)
		{
			continue;
		}
		for (int k = 0; k < fairy->GetSkillHitNum(); k++)
		{
			VECTOR skillPos = fairy->GetSkillHitPos(k);
			float skillRadius = fairy->GetSkillHitRadius(k);
			for (int j = 0; j < ENEMY_MAX; j++)
			{
				Enemy& enemy = enemyManager.GetEnemy(j);
				if (enemy.GetActive() == false)
				{
					continue;
				}

				//敵とそれぞれの精霊のスキルの当たり判定
				if (Collsion::CheckHitSphereToSphere(skillPos, skillRadius, enemy.GetCollisionPos(), enemy.GetRadius()) == true)
				{
					ApplySkillEffect(fairy, enemy,k);
				}
			}
		}
	}
}