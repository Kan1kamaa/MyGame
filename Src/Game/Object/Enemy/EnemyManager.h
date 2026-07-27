#pragma once
#include"Enemy.h"

//敵の数の最大値
static const int ENEMY_MAX = 50;

class EnemyManager {
private:
	Enemy m_Enemy[ENEMY_MAX];		//敵
	int m_waitCnt;					//次の敵が出るまでの時間
	int m_EnemyCnt;
public:
	//コンストラクタ・デストラクタ
	EnemyManager();
	~EnemyManager();
	
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

	
	//@pos : 発射する座標
	//@speed : 移動速度
	//@return : true = 生成成功 false = 失敗
	bool RequestEnemy();
	//個別の敵データ取得
	Enemy& GetEnemy(int index) { return m_Enemy[index]; }
};