#pragma once
#include<DxLib.h>

class DebugCamera {
private:
	VECTOR m_camerapos;
	VECTOR m_focuspos;
	VECTOR m_upvec;
	VECTOR m_speed;
	VECTOR m_camerarot;
public:
	//コンストラクタ・デストラクタ
	DebugCamera();
	~DebugCamera();

	void Init();

	void Step(VECTOR targetpos, float roty);

	void Draw();

	void Update();
};