#include <crtdbg.h>
#include "DxLib.h"
#include "../Src/Lib/Fps/Fps.h"
#include"../Src/Game/Scene/SceneManager.h"
// �v���O������ WinMain ����n�܂�܂�
int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// ���������[�N�m�F�p
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ChangeWindowMode(TRUE);			// �E�B���h�E���[�h�ŋN��
	SetGraphMode(1280, 720, 32);

	// �c�w���C�u��������������
	if (DxLib_Init() == -1) return -1;

	//��ԍŏ��ɂP�񂾂���鏈��
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	// ������
	InitFps();
	SceneManager scene;
	//�Q�[�����C�����[�v
	while (ProcessMessage() != -1)
	{
		//�G�X�P�[�v�L�[�������ꂽ��I��
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
		if (!IsNextFrame()) continue;

		ClearDrawScreen();
		
		//�X�V����
		scene.Loop();
		//�`��
		scene.Draw();
		PrintFps();		// FPS�\��

		ScreenFlip();

		StepFps();		// frame timer base update (missing call caused framerate-dependent speed)

	}

	
	DxLib_End();			// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}

