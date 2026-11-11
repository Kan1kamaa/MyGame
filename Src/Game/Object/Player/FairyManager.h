#pragma once
#include "WindFairy.h"
#include "FireFairy.h"
#include "ThunderFairy.h"
#include "WaterFairy.h"
#include<vector>

//精霊を管理するクラス
class FairyManager {
private:
	enum FairyType {
		NONE,
		WIND,
		FIRE,
		WATER,
		THUNDER,

		FAIRYTYPENUM
	};
	FairyBase	*m_nowFairy;
	std::vector<FairyBase*> m_LivingFairy;
	FairyType m_nowType;
	//複製の元になる各属性のモデルハンドル(FAIRYTYPENUM個。NONEの分は使わない)
	int m_modelHndl[FAIRYTYPENUM];

	//前フレームのキー入力状態(押した瞬間だけ発動させるための判定用)
	bool m_prevKeyE;
	bool m_prevKey1;
	bool m_prevKey2;
	bool m_prevKey3;
	bool m_prevKey4;
	void SwapFairy(FairyType,VECTOR playerPos);
	//場にいる精霊を全員片付ける(元モデルは残す)
	void ClearFairy();
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
	void Step(const VECTOR& playerPos,const float playerRotY);
	//情報更新
	void Update();
	//描画
	void Draw();
	//リスポーンなどで、精霊だけを最初の状態に戻す(元モデルは残す)
	void Reset();
	//破棄(元モデルも消す)
	void Fin();
	//今いる精霊の数を返す
	int GetFairyCount()const;

	FairyBase* GetFairy(int index)const;

	bool PopEffectRequest(EffectManager::EffectId& id, VECTOR& pos, float& rotY);
};
