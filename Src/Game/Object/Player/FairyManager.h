#pragma once
#include "WindFairy.h"
#include "FireFairy.h"
#include "ThunderFairy.h"
#include<vector>

//精霊を管理するクラス
class FairyManager {
private:
	enum FairyType {
		NONE,
		WIND,
		FIRE,
		THUNDER,

		FAIRYTYPENUM
	};
	FairyBase	*m_nowFairy;
	std::vector<FairyBase*> m_LivingFairy;
	FairyType m_nowType;

	//前フレームのキー入力状態(押した瞬間だけ発動させるための判定用)
	bool m_prevKeyE;
	bool m_prevKey1;
	bool m_prevKey2;
	bool m_prevKey3;

	void SwapFairy(FairyType,VECTOR playerPos);
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
