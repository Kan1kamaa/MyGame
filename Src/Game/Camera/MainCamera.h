#pragma once
#include<DxLib.h>

class MainCamera {
private:
	VECTOR m_camerapos;
	VECTOR m_focuspos;
	VECTOR m_upvec;

	float m_yaw;      //カメラの水平方向の向き(マウスで操作)
	float m_pitch;    //カメラの垂直方向の向き(マウスで操作)
	float m_distance; //注視点からの距離

public:
	//コンストラクタ・デストラクタ
	MainCamera();
	~MainCamera();

	void Init();

	//マウスの動きでカメラの向き(m_yaw/m_pitch)だけを更新する
	void UpdateLook();
	//注視点(targetPos)を中心に、現在の向きに応じたカメラ座標を計算する
	void Step(VECTOR targetPos);

	void Update();

	//現在のカメラの水平方向の向き(移動をカメラ基準にする際に使う)
	float GetYaw() const { return m_yaw; }
};
