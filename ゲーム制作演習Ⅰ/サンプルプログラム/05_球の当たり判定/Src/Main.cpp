#include <crtdbg.h>
#include "DxLib.h"
#include "Fps/Fps.h"

//---------------------------------
// 箱同士の当たり判定
//---------------------------------
bool CheckHitBoxToBox(VECTOR boxPos1, VECTOR size1,
	VECTOR boxPos2, VECTOR size2)
{
	// 箱の上下左右奥手前それぞれの座標を計算する
	float up1 = boxPos1.y + size1.y * 0.5f;
	float down1 = boxPos1.y - size1.y * 0.5f;
	float left1 = boxPos1.x - size1.x * 0.5f;
	float right1 = boxPos1.x + size1.x * 0.5f;
	float front1 = boxPos1.z - size1.z * 0.5f;
	float back1 = boxPos1.z + size1.z * 0.5f;

	float up2 = boxPos2.y + size2.y * 0.5f;
	float down2 = boxPos2.y - size2.y * 0.5f;
	float left2 = boxPos2.x - size2.x * 0.5f;
	float right2 = boxPos2.x + size2.x * 0.5f;
	float front2 = boxPos2.z - size2.z * 0.5f;
	float back2 = boxPos2.z + size2.z * 0.5f;

	// 4つの端をそれぞれチェックして、すべての条件を満たしたらヒット！
	if (left1 <= right2 && right1 >= left2
		&& up1 >= down2 && down1 <= up2)
	{
		// 最後に奥行きもチェック
		if (front1 <= back2 && back1 >= front2)
		{
			return true;
		}
	}

	return false;
}

//---------------------------------
// 球同士の当たり判定
//---------------------------------
bool CheckHitSphereToSphere(VECTOR spherePos1, float radius1,
	VECTOR spherePos2, float radius2)
{
	// 円1から円2までの距離を計算
	float lengthX = spherePos1.x - spherePos2.x;	// どうせ2乗するとマイナスが消えるので、順番はどうでもいい
	lengthX *= lengthX;
	float lengthY = spherePos1.y - spherePos2.y;	// どうせ2乗するとマイナスが消えるので、順番はどうでもいい
	lengthY *= lengthY;
	float lengthZ = spherePos1.z - spherePos2.z;	// どうせ2乗するとマイナスが消えるので、順番はどうでもいい
	lengthZ *= lengthZ;
	float length = lengthX + lengthY + lengthZ;	// これがaの2乗＋bの2乗

	// 2つの円の半径を加算し、2乗する
	float lengthRadius = (float)(radius1 + radius2);
	lengthRadius *= lengthRadius;

	// 以下の条件を満たせばヒットする！
	if (lengthRadius >= length)
	{
		return true;
	}
	else return false;
}

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

	// 初期化
	InitFps();

	// モデルのロード
	int hndl1 = MV1LoadModel("Data/Models/HitSphere/SphereChan.x");
	int hndl2 = MV1LoadModel("Data/Models/HitSphere/SphereChanBig.x");

	// キャラクターの座標
	VECTOR pos1 = { 5.0f, 0.0f, 0.0f };
	VECTOR pos2 = { -5.0f, 0.0f, 0.0f };

	// カメラ情報設定
	VECTOR camPos = { 0.0f, 20.0f, -30.0f };
	VECTOR focusPos = { 0.0f, 0.0f, 0.0f };
	VECTOR upVec = { 0.0f, 1.0f, 0.0f };

	// カメラのニアファーを設定
	SetCameraNearFar(1.0f, 100.0f);

	//ゲームメインループ
	while (ProcessMessage() != -1)
	{
		//エスケープキーが押されたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
		if (!IsNextFrame()) continue;

		ClearDrawScreen();

		// 更新処理
		// キー入力による移動
		float SPEED = 1.0f;		// 後で移動速度が調整しやすいように
		// 左右移動
		if (CheckHitKey(KEY_INPUT_RIGHT)) pos1.x += SPEED;
		else if (CheckHitKey(KEY_INPUT_LEFT)) pos1.x -= SPEED;
		// 上下移動
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			if (CheckHitKey(KEY_INPUT_UP)) pos1.y += SPEED;
			else if (CheckHitKey(KEY_INPUT_DOWN)) pos1.y -= SPEED;
		}
		// 前後移動
		else
		{
			if (CheckHitKey(KEY_INPUT_UP)) pos1.z += SPEED;
			else if (CheckHitKey(KEY_INPUT_DOWN)) pos1.z -= SPEED;
		}

		// 左右移動
		if (CheckHitKey(KEY_INPUT_D)) pos2.x += SPEED;
		else if (CheckHitKey(KEY_INPUT_A)) pos2.x -= SPEED;
		// 上下移動
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			if (CheckHitKey(KEY_INPUT_W)) pos2.y += SPEED;
			else if (CheckHitKey(KEY_INPUT_S)) pos2.y -= SPEED;
		}
		// 前後移動
		else
		{
			if (CheckHitKey(KEY_INPUT_W)) pos2.z += SPEED;
			else if (CheckHitKey(KEY_INPUT_S)) pos2.z -= SPEED;
		}

		// 移動後に座標をセット
		MV1SetPosition(hndl1, pos1);
		MV1SetPosition(hndl2, pos2);

		// カメラの位置を更新
		SetCameraPositionAndTargetAndUpVec(
			camPos, focusPos, upVec
		);

		// 描画処理
		MV1DrawModel(hndl1);
		MV1DrawModel(hndl2);

		// 当たり判定結果を表示
		bool isHit = CheckHitSphereToSphere(pos1, 2.0f, pos2, 4.0f);
		if (isHit == true)
		{
			DrawFormatString(32, 32, GetColor(255, 255, 255),
				"Hit!!");
		}

		PrintFps();		// 最後にFPS表示

		ScreenFlip();

	}

	// 終了処理
	MV1DeleteModel(hndl1);
	MV1DeleteModel(hndl2);
	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

