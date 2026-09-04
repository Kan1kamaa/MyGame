#pragma once
#include"../ObjectBase/ObjectBase.h"

class Enemy : public ObjectBase {
private:
	VECTOR m_speed;		//移動速度
	float  m_speedMag;		// speed magnitude, kept constant across direction changes
	int    m_dirChangeCnt;	// frames remaining until next random direction change
	void ChangeDirRandom();	// pick a new random movement direction, keeping speed magnitude
public:
	//コンストラクタ・デストラクタ
	Enemy();
	~Enemy();

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

	void HitCalc(const ObjectBase& other);
};
