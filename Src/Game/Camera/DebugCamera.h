#pragma once
#include<DxLib.h>

class DebugCamera {
private:
	VECTOR m_cameraPos;
	VECTOR m_focusPos;
	VECTOR m_upVec;
	VECTOR m_speed;
	VECTOR m_cameraRot;
public:
	//コンストラクタ・デストラクタ
	DebugCamera();
	~DebugCamera();

	void Init();

	void Step(VECTOR targetpos, float roty);

	void Draw();

	void Update();
};