#pragma once
#include"Scenebase.h"
#include"../Camera/CameraManager.h"
#include"../Object/Field/Sky.h"
#include "../Object/Field/Field.h"
#include"../Object/Player/CharacterManager.h"
#include"../Object/Enemy/EnemyManager.h"
#include"../Object/Enemy/Boss.h"
#include"../System/GameCollision.h"
#include"../System/SoundManager.h"
#include"../../Lib/Effect/EffectManager.h"
class PlayScene : public SceneBase {
private:
	//エフェクト(Effekseer)
	EffectManager m_effect;
	//プレイヤー
	CharacterManager m_player;
	//弾
	ShotManager m_shot;
	//敵
	EnemyManager m_enemy;
	//ボス
	BossGolem m_boss;
	//背景
	Field m_field;
	Sky m_sky;
	//カメラ
	CameraManager m_camera;


	
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
