#include <crtdbg.h>
#include "DxLib.h"
#include "../Src/Lib/Fps/Fps.h"
#include"../Src/Game/Scene/SceneManager.h"
// プログラムは WinMain から始まります
int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// メモリリーク確認用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ChangeWindowMode(TRUE);			// ウィンドウモードで起動
	SetGraphMode(1280, 720, 32);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1) return -1;

	//一番最初に１回だけやる処理
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	// 初期化
	InitFps();
	SceneManager scene;
	//ゲームメインループ
	while (ProcessMessage() != -1)
	{
		//エスケープキーが押されたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
		if (!IsNextFrame()) continue;

		ClearDrawScreen();
		
		//更新処理
		scene.Loop();
		//描画
		scene.Draw();
		PrintFps();		// FPS表示

		ScreenFlip();

	}

	
	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

