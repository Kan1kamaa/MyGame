#pragma once
#include"Scenebase.h"
#include"../Camera/CameraManager.h"
#include"../Object/Field/Sky.h"
#include "../Object/Field/Field.h"
#include"../Object/Player/CharacterManager.h"
#include"../Object/Enemy/EnemyManager.h"
#include"../System/GameCollision.h"
#include"../System/SoundManager.h"
class PlayScene : public SceneBase {
private:
	//プレイヤー
	CharacterManager player;
	//弾
	ShotManager shot;
	//敵
	EnemyManager enemy;
	//背景
	Field field;
	Sky sky;
	//カメラ
	CameraManager camera;

public:
	//描画
	void Draw();

private:
	//初期化
	void Init();
	//ロード
	void Load();
	//毎フレーム計算する処理
	void Step();
	//情報更新
	void Update();
	//破棄
	void Fin();
};
