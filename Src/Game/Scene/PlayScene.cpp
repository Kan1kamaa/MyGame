#include"PlayScene.h"

//ボスの出現座標(プレイヤーのスポーン地点から少し離れた場所)
static const VECTOR BOSS_SPAWN_POS = { 0.0f, 0.0f, 300.0f };

//シャドウマップ(サイズは解像度に関係する。必ず2の階乗)
static const int SHADOW_MAP_SIZE = 2048;
//光の方向(シャドウマップと通常のライトで同じ向きにそろえる)
static const VECTOR LIGHT_DIRECTION = { 1.0f, -1.0f, 0.0f };
//影を描く範囲(プレイヤーを中心にした、水平方向の半径と高さの下限・上限)
static const float SHADOW_RANGE = 100.0f;
static const float SHADOW_MIN_Y = -5.0f;
static const float SHADOW_MAX_Y = 80.0f;


// 描画処理
void PlayScene::Draw()
{
	//影を描く範囲を、プレイヤーの周りに合わせる(離れた場所でも影が出るように毎フレーム更新)
	VECTOR playerPos = player.GetPos();
	SetShadowMapDrawArea(shadowHndl,
		VGet(playerPos.x - SHADOW_RANGE, SHADOW_MIN_Y, playerPos.z - SHADOW_RANGE),
		VGet(playerPos.x + SHADOW_RANGE, SHADOW_MAX_Y, playerPos.z + SHADOW_RANGE));

	//影用の描画(影を落とすものだけを描く。地面は含めない)
	ShadowMap_DrawSetup(shadowHndl);
	player.DrawPL();
	enemy.Draw();
	boss.Draw();
	ShadowMap_DrawEnd();

	//通常の描画(シャドウマップを使って、影を受けるものを描く)
	SetUseShadowMap(0, shadowHndl);
	field.Draw();
	player.DrawPL();
	enemy.Draw();
	boss.Draw();
	SetUseShadowMap(0, -1);

	sky.Draw();
	shot.Draw();
	camera.Draw();
	//HUDは最後に描いて、他の描画に隠れないようにする
	player.DrawHUD();
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

	//シャドウマップを作る(まだ作っていない時だけ)
	if (shadowHndl == -1)
	{
		shadowHndl = MakeShadowMap(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	}

	//光の方向を設定する(影の向きと、モデルの明暗の向きをそろえる)
	SetShadowMapLightDirection(shadowHndl, LIGHT_DIRECTION);
	SetLightDirection(LIGHT_DIRECTION);
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
		player.Step(camera.GetYaw());
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
	GameCollision::CheckHitBossAttackToPlayer(boss, player);
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

	//シャドウマップを破棄する
	if (shadowHndl != -1)
	{
		DeleteShadowMap(shadowHndl);
		shadowHndl = -1;
	}

	m_nextScene = 0;	//とりあえずリザルトに
	m_state = INIT;		//念のため最初に戻す
}
