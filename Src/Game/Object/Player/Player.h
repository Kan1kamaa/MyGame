#pragma once
#include"../ObjectBase/ObjectBase.h"
#include "../ActorBase/ActorBase.h"
#include"../Shot/ShotManager.h"

//プレイヤーを表示する
class Player : public ActorBase {

public:

	//歩きサウンドを鳴らすかフラグ
	bool   m_isMoveSE;
	VECTOR m_effectPos;
	ActorBase m_actor2;
	int m_hndl2;
	int m_state;
	//移動速度
	VECTOR m_speed;

	//================================
	// アニメーションに関する関数たち

	// 何もしていないとき
	void IdleExec();
	// 歩き中
	void WalkExec();
	// 走り中
	void RunExec();
	// 攻撃中
	void AttackExec();
	// 何もしていないとき
	void IdleExec2();
	// 歩き中
	void WalkExec2();
	// 走り中
	void RunExec2();
	// 攻撃中
	void AttackExec2();



	//================================

public:
	//コンストラクタ・デストラクタ
	Player();
	~Player();

	//初期化処理
	void Init();
	// データロード
	void Load();
	//毎フレーム計算する処理
	void Step(ShotManager& shotManager);

	void DrawPL();

	void Update();
};