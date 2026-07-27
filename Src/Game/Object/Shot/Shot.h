#pragma once
#include"../ObjectBase/ObjectBase.h"

class Shot : public ObjectBase {
private:
	VECTOR m_speed;		//移動速度
public:
	//コンストラクタ・デストラクタ
	Shot();
	~Shot();

	//初期化
	void Init();
	//ロード
	void Load(int originhndl);
	//毎フレーム計算する処理
	void Step();
	//ショット発射
	//@pos : 発射する座標
	//@sped :　移動速度
	//@return : true = 生成成功 false = 失敗
	bool Request(const VECTOR& pos, const VECTOR& speed);
	//当たり判定の中心を取得
	VECTOR GetCollisionPos(){ return m_pos; }
};