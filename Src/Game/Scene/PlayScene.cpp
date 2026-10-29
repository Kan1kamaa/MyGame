#include"PlayScene.h"

//ボスの出現座標(プレイヤーのスポーン地点から少し離れた場所)
static const VECTOR BOSS_SPAWN_POS = { 0.0f, 0.0f, 300.0f };


// 描画処理
void PlayScene::Draw()
{
	field.Draw();
	sky.Draw();
	player.DrawPL();
	fairy.Draw();
	player.DrawHUD();
	shot.Draw();
	camera.Draw();
	enemy.Draw();
	boss.Draw();
	effect.Draw();
}

//初期化
void PlayScene::Init()
{
	field.Init();
	sky.Init();
	player.Init();
	fairy.Init();
	camera.Init();
	shot.Init();
	enemy.Init();
	boss.Init();
	effect.Init();

	m_state = LOAD;
}

//ロード
void PlayScene::Load()
{
	field.Load();
	sky.Load();
	enemy.Load();
	boss.Load();
	player.Load();
	fairy.Load();
	shot.Load();
	effect.Load();
	boss.Request(BOSS_SPAWN_POS);
	Step();
	Update();
	//BGMは一旦オフ
	//SoundManager::Play(SoundManager::GAME_BGM, DX_PLAYTYPE_LOOP);
	m_state = START;
}

//毎フレーム計算する処理
void PlayScene::Step()
{
	if (camera.GetCameraID() == 0)
	{
		//先にマウスでカメラを回し、移動がこのフレームのカメラ向きを使うようにする
		camera.UpdateLook();
		player.Step(camera.GetYaw());
		fairy.Step(player.GetPos());
		shot.Step();
		enemy.Step(player.GetPos());
		boss.Step(player.GetPos());
		sky.Step();

		//攻撃・スキル・必殺技が始まったらエフェクトを出す
		EffectManager::EffectId effectId;
		VECTOR effectPos;
		float effectRotY;
		if (player.PopEffectRequest(effectId, effectPos, effectRotY) == true)
		{
			effect.Play(effectId, effectPos, effectRotY);
		}
	}
	//カメラの更新
	camera.Step(player.GetPos());
	//各種当たり判定
	GameCollision::CheckHitEnemyToShot(enemy, shot);
	GameCollision::CheckHitEnemyToPlayer(enemy, player);
	GameCollision::CheckHitPlayerAttackToEnemy(player, enemy);
	GameCollision::CheckHitBossToShot(boss, shot);
	GameCollision::CheckHitBossAttackToPlayer(boss, player);
	GameCollision::CheckHitPlayerAttackToBoss(player, boss);

	//プレイヤーがリスポーンしたら、ボスも最初の状態に戻して出し直す
	if (player.CheckRespawned() == true)
	{
		boss.Reset(BOSS_SPAWN_POS);
		fairy.Reset();
	}
}

// 更新処理
void PlayScene::Update()
{
	field.Update();
	sky.Update(player.GetPos());
	player.Update();
	fairy.Update();
	shot.Update();
	enemy.Update();
	boss.Update();
	camera.Update();
	effect.Update();
}

// 終了処理
void PlayScene::Fin()
{
	field.Fin();
	sky.Fin();
	player.Fin();
	fairy.Fin();
	shot.Fin();
	enemy.Fin();
	boss.Fin();
	effect.Fin();

	m_nextScene = 0;	//とりあえずリザルトに
	m_state = INIT;		//念のため最初に戻す
}
