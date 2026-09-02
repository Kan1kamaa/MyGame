#pragma once
#include"MainCamera.h"
#include"DebugCamera.h"
class CameraManager {
public:
	//�g�p�ł���J�����̃^�C�v
	enum tagCameraID
	{
		MAIN,		//�Q�[�����̃��C���J����
		DEBUG,		//�f�o�b�O�p�̃J����
	};
private:
	tagCameraID m_state;		//���݂̃J�������
	MainCamera m_mainCam;		//���C���J����
	DebugCamera m_debugCam;
public:
	//�R���X�g���N�^
	CameraManager();
	//������
	void Init();

	void Draw();
	//update camera look direction from mouse (MAIN camera only)
	void UpdateLook();
	//���t���[���v�Z���鏈��
	void Step(VECTOR targetPos);
	//���X�V
	void Update();

	//�J�����^�C�v�̃Z�b�g�Q�b�g
	void setCameraID(tagCameraID& state) { m_state&  state; }
	tagCameraID GetCameraID(){ return m_state; }
	//current camera yaw, used to make player movement camera-relative
	float GetYaw() const { return m_mainCam.GetYaw(); }
};