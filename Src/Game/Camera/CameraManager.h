#pragma once
#include"MainCamera.h"
#include"DebugCamera.h"
class CameraManager {
public:
	//使用できるカメラのタイプ
	enum tagCameraID
	{
		MAIN,		//ゲーム中のメインカメラ
		DEBUG,		//デバッグ用のカメラ
	};
private:
	tagCameraID m_state;		//現在のカメラ状態
	MainCamera m_mainCam;		//メインカメラ
	DebugCamera m_debugCam;
public:
	//コンストラクタ
	CameraManager();
	//初期化
	void Init();

	void Draw();
	//マウス入力でカメラの向きを更新する(MAINカメラのみ)
	void UpdateLook();
	//毎フレーム計算する処理
	void Step(VECTOR targetPos);
	//情報更新
	void Update();

	//カメラタイプのセットゲット
	void setCameraID(tagCameraID& state) { m_state&  state; }
	tagCameraID GetCameraID(){ return m_state; }
	//プレイヤーの移動をカメラ基準にするために使う、現在のカメラの水平回転角
	float GetYaw() const { return m_mainCam.GetYaw(); }
};
