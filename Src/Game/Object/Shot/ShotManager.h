#pragma once
#include"Shot.h"

//プレイヤーの弾の最大値
static const int PL_SHOT_MAX = 500;
static const int SHOT_SPEED = -5;
class ShotManager {
private:
	//プレイヤー用の弾
	Shot m_playerShot[PL_SHOT_MAX];

public:
	//コンストラクタ・デストラクタ
	ShotManager();
	~ShotManager();

	//初期化
	void Init();
	//ロード
	void Load();
	//毎フレーム計算する処理
	void Step();
	//情報更新
	void Update();
	//描画
	void Draw();
	//破棄
	void Fin();


	//プレイヤー攻撃処理
	//@pos	: 発射する座標
	//@speed: 移動速度
	//return: ture = 生成成功　false = 失敗
	bool RequestPlayerAttack(const VECTOR& pos, const VECTOR& speed);
	//プレイヤーショット発射
	//@pos : 発射する座標
	//@speed : 移動速度
	//@return : true = 生成成功 false = 失敗
	bool RequestPlayerShot(const VECTOR& pos, const VECTOR& speed);
	//個別の弾データ取得
	Shot & GetPlayerShot(int index){return m_playerShot[index];}


};