#include"PlayScene.h"

// 描画処理
void PlayScene::Draw()
{
	field.Draw();
	sky.Draw();
	player.DrawPL();
	shot.Draw();
	camera.Draw();
	enemy.Draw();
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

	m_state = LOAD;
}

//ロード
void PlayScene::Load()
{
	field.Load();
	sky.Load();
	enemy.Load();
	player.Load();
	shot.Load();
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
		//mouse look first, so movement uses this frame's camera direction
		camera.UpdateLook();
		player.Step(shot, camera.GetYaw());
		shot.Step();
		enemy.Step();
		sky.Step();
	}
	//カメラの更新
	camera.Step(player.GetPos());
	//各種当たり判定
	GameCollision::CheckHitEnemyToShot(enemy, shot);
	GameCollision::CheckHitEnemyToPlayer(enemy, player);

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

	m_nextScene = 0;	//とりあえずリザルトに
	m_state = INIT;		//念のため最初に戻す
}
