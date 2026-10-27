#pragma once
#include "WindFairy.h"
#include "FireFairy.h"
#include "ThunderFairy.h"

//精霊を管理するクラス
class FairyManager {
public:
	//コンストラクタ・デストラクタ
	FairyManager();
	~FairyManager();

	//初期化
	void Init();
	//ロード
	void Load();
	//毎フレーム計算する処理
	//@playerPos : プレイヤーの座標
	void Step(const VECTOR& playerPos);
	//情報更新
	void Update();
	//描画
	void Draw();
	//破棄
	void Fin();
};
