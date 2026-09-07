#pragma once
#include"../ObjectBase/ObjectBase.h"

class Enemy : public ObjectBase {
private:
	VECTOR m_speed;		//移動速度
	int m_changeDirCnt;	//次にランダムで方向転換するまでのフレーム数

	//移動方向をランダムに選び直す
	void RandomizeDirection();
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
	//@speed :　移動速度
	//@return : true = 生成成功 false = 失敗
	bool Request(const VECTOR& pos, const VECTOR& speed);

	void HitCalc(const ObjectBase& other);
};
