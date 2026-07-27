#pragma once
#include<DxLib.h>

class MainCamera {
private:
	VECTOR m_camerapos;
	VECTOR m_focuspos;
	VECTOR m_upvec;
public:
	//コンストラクタ・デストラクタ
	MainCamera();
	~MainCamera();

	void Init();

	void Step(VECTOR targetpos,float roty);

	void Update();
};