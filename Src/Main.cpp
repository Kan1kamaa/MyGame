#include <crtdbg.h>
#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "../Src/Lib/Fps/Fps.h"
#include"../Src/Game/Scene/SceneManager.h"
// プログラムは WinMain から始まります
int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// メモリリーク確認用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ChangeWindowMode(true);			// ウィンドウモードで起動
	SetGraphMode(1280, 720, 32);
	SetUseDirect3DVersion(DX_DIRECT3D_11);	// Effekseerを使うためDirectX11を指定

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1) return -1;

	// Effekseerの初期化(引数は画面に表示する最大パーティクル数)
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
		return -1;
	}
	// フルスクリーン切り替え時にリソースが消えないようにする
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	// デバイスロスト時にEffekseer側のリソースを復帰させる
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

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
		if (IsNextFrame() == false) continue;

		ClearDrawScreen();

		//更新処理
		scene.Loop();
		//描画
		scene.Draw();
		PrintFps();		// FPS表示

		ScreenFlip();
		StepFps();

	}


	Effkseer_End();			// Effekseerの終了処理(関数名はライブラリ側の綴りのまま)
	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了
}
