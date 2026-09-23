#include"PlayScene.h"

//ボスの出現座標(プレイヤーのスポーン地点から少し離れた場所)
static const VECTOR BOSS_SPAWN_POS = { 0.0f, 0.0f, 200.0f };

// 描画処理
void PlayScene::Draw()
{
	field.Draw();
	sky.Draw();
	player.DrawPL();
	shot.Draw();
	camera.Draw();
	enemy.Draw();
	boss.Draw();
}

//初期化
void PlayScene::Init()
{
	field.Init();
	sky.Init();
	player.Init();
	camera.Init();
	shot.Init();
	enemy.Init();
	boss.Init();

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
	shot.Load();
	boss.Request(BOSS_SPAWN_POS);
	Step();
	Update();
	SoundManager::Play(SoundManager::GAME_BGM, DX_PLAYTYPE_LOOP);
	m_state = START;
}

//毎フレーム計算する処理
void PlayScene::Step()
{
	if (camera.GetCameraID() == 0)
	{
		//先にマウスでカメラを回し、移動がこのフレームのカメラ向きを使うようにする
		camera.UpdateLook();
		player.Step(shot, camera.GetYaw());
		shot.Step();
		enemy.Step(player.GetPos());
		boss.Step(player.GetPos());
		sky.Step();
	}
	//カメラの更新
	camera.Step(player.GetPos());
	//各種当たり判定
	GameCollision::CheckHitEnemyToShot(enemy, shot);
	GameCollision::CheckHitEnemyToPlayer(enemy, player);
	GameCollision::CheckHitPlayerAttackToEnemy(player, enemy);
	GameCollision::CheckHitBossToShot(boss, shot);
	GameCollision::CheckHitBossToPlayer(boss, player);
	GameCollision::CheckHitPlayerAttackToBoss(player, boss);

	if (player.GetActive() == false)
	{
		SoundManager::StopAll();
		m_state = END_WAIT;
	}
}

// 更新処理
void PlayScene::Update()
{
	field.Update();
	sky.Update(player.GetPos());
	player.Update();
	shot.Update();
	enemy.Update();
	boss.Update();
	camera.Update();
}

// 終了処理
void PlayScene::Fin()
{
	field.Fin();
	sky.Fin();
	player.Fin();
	shot.Fin();
	enemy.Fin();
	boss.Fin();

	m_nextScene = 0;	//とりあえずリザルトに
	m_state = INIT;		//念のため最初に戻す
}
