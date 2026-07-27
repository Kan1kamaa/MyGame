#include"CameraManager.h"

//コンストラクタ
CameraManager::CameraManager() : m_state(MAIN)
{
}

//初期化
void CameraManager::Init()
{
	m_state = MAIN;
	m_mainCam.Init();
	m_debugCam.Init();
}
void CameraManager::Draw()
{
	switch (m_state)
	{
	case DEBUG:
		m_debugCam.Draw();
		break;
	}
}
//毎フレーム計算する処理
void CameraManager::Step(VECTOR tergetPos, float rotY)
{
	if (CheckHitKey(KEY_INPUT_V) == true)
	{
		m_state = MAIN;
	}
	else if (CheckHitKey(KEY_INPUT_C) == true)
	{
		m_state = DEBUG;
		m_debugCam.Init();
	}
	switch(m_state)
	{
		case MAIN:
			m_mainCam.Step( tergetPos,  rotY);
			break;
		case DEBUG:
			m_debugCam.Step(tergetPos,  rotY);
			break;

	}
}
void  CameraManager::Update()
{
	switch (m_state)
	{
	case MAIN:
		m_mainCam.Update();
		break;
	case DEBUG:
		m_debugCam.Update();
		break;

	}
}