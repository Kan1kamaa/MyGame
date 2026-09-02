#include"CameraManager.h"

//�R���X�g���N�^
CameraManager::CameraManager() : m_state(MAIN)
{
}

//������
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
//update camera look direction from mouse (MAIN camera only)
void CameraManager::UpdateLook()
{
	if (m_state == MAIN)
	{
		m_mainCam.UpdateLook();
	}
}

//���t���[���v�Z���鏈��
void CameraManager::Step(VECTOR tergetPos)
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
			m_mainCam.Step(tergetPos);
			break;
		case DEBUG:
			m_debugCam.Step(tergetPos, 0.0f);
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