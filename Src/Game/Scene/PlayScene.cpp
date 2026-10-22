#include"PlayScene.h"

//ボスの出現座標(プレイヤーのスポーン地点から少し離れた場所)
static const VECTOR BOSS_SPAWN_POS = { 0.0f, 0.0f, 300.0f };


// 描画処理
void PlayScene::Draw()
{
	m_field.Draw();
	m_sky.Draw();
	m_player.DrawPL();
	m_player.DrawHUD();
	m_shot.Draw();
	m_camera.Draw();
	m_enemy.Draw();
	m_boss.Draw();
	m_effect.Draw();
}

//初期化
void PlayScene::Init()
{
	m_field.Init();
	m_sky.Init();
	m_player.Init();
	m_camera.Init();
	m_shot.Init();
	m_enemy.Init();
	m_boss.Init();
	m_effect.Init();

	m_state = LOAD;
}

//ロード
void PlayScene::Load()
{
	m_field.Load();
	m_sky.Load();
	m_enemy.Load();
	m_boss.Load();
	m_player.Load();
	m_shot.Load();
	m_effect.Load();
	m_boss.Request(BOSS_SPAWN_POS);
	Step();
	Update();
	//BGMは一旦オフ
	//SoundManager::Play(SoundManager::GAME_BGM, DX_PLAYTYPE_LOOP);
	m_state = START;
}

//毎フレーム計算する処理
void PlayScene::Step()
{
	if (m_camera.GetCameraID() == CameraManager::MAIN)
	{
		//先にマウスでカメラを回し、移動がこのフレームのカメラ向きを使うようにする
		m_camera.UpdateLook();
		m_player.Step(m_camera.GetYaw());
		m_shot.Step();
		m_enemy.Step(m_player.GetPos());
		m_boss.Step(m_player.GetPos());
		m_sky.Step();

		//攻撃・スキル・必殺技が始まったらエフェクトを出す
		EffectManager::EffectId effectId;
		VECTOR effectPos;
		float effectRotY;
		if (m_player.PopEffectRequest(effectId, effectPos, effectRotY) == true)
		{
			m_effect.Play(effectId, effectPos, effectRotY);
		}
	}
	//カメラの更新
	m_camera.Step(m_player.GetPos());
	//各種当たり判定
	GameCollision::CheckHitEnemyToShot(m_enemy, m_shot);
	GameCollision::CheckHitEnemyToPlayer(m_enemy, m_player);
	GameCollision::CheckHitPlayerAttackToEnemy(m_player, m_enemy);
	GameCollision::CheckHitBossToShot(m_boss, m_shot);
	GameCollision::CheckHitBossAttackToPlayer(m_boss, m_player);
	GameCollision::CheckHitPlayerAttackToBoss(m_player, m_boss);

	//プレイヤーがリスポーンしたら、ボスも最初の状態に戻して出し直す
	if (m_player.CheckRespawned() == true)
	{
		m_boss.Reset(BOSS_SPAWN_POS);
	}
}

// 更新処理
void PlayScene::Update()
{
	m_field.Update();
	m_sky.Update(m_player.GetPos());
	m_player.Update();
	m_shot.Update();
	m_enemy.Update();
	m_boss.Update();
	m_camera.Update();
	m_effect.Update();
}

// 終了処理
void PlayScene::Fin()
{
	m_field.Fin();
	m_sky.Fin();
	m_player.Fin();
	m_shot.Fin();
	m_enemy.Fin();
	m_boss.Fin();
	m_effect.Fin();

	m_nextScene = 0;	//とりあえずリザルトに
	m_state = INIT;		//念のため最初に戻す
}
