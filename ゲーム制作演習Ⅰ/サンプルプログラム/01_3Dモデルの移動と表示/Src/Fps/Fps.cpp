#include <DxLib.h>

// これも定義しておくと、フレームレート自体の変更が可能に
#define FRAME_RATE (60)								// 1秒辺りのフレーム数
#define FRAME_RATE_MILLI_SECOND (1000 / FRAME_RATE)	// 何ミリ秒で1フレームか

// FPS表示用構造体
typedef struct{
	double m_drawFps;	// 実際に表示するfps
	int m_nowTime;		// 現在のPC時間
	int m_prevTime;		// 1秒前のPC時間
	int m_prevDrawTime;	// 1つ前の描画したPC時間
	int m_count;		// カウント用
}FpsData;

// 上記構造体を基に、グローバル変数を作成
static FpsData g_fps = { 0.0 };


//-------------------------------
//		変数初期化
//-------------------------------
void InitFps()
{
	g_fps.m_drawFps = 0.0;
	g_fps.m_nowTime = 0;
	// 一旦PC時間を保存しておく
	g_fps.m_prevTime = g_fps.m_prevDrawTime = GetNowCount();
	g_fps.m_count = 0;
}


//-------------------------------
//		次のフレームに進めていいか
//-------------------------------
bool IsNextFrame()
{
	// 最新の時間を取得し更新
	g_fps.m_nowTime = GetNowCount();
	// 前回の時間から今回までの差分を取得
	int difTime = g_fps.m_nowTime - g_fps.m_prevTime;

	// 前回の時間から指定したフレーム分を経過したかチェック
	if (difTime >= FRAME_RATE_MILLI_SECOND)
	{
		return true;
	}
	else return false;
}


//-------------------------------
//		FPS更新処理
//-------------------------------
void StepFps()
{
	// 1フレーム前の時間を更新するだけ
	g_fps.m_prevTime = g_fps.m_nowTime;
}


//-------------------------------
//		FPS表示関数
//-------------------------------
void PrintFps()
{
	// 想定通りなら1フレーム進んだはず
	g_fps.m_count++;
	// 前回描画更新したときの時間と今回の時間の差を取得
	int difTime = g_fps.m_nowTime - g_fps.m_prevDrawTime;

	// 一秒以上経過していたら、FPS計算開始
	if (difTime >= 1000)
	{
		// 想定通りなら1000ミリ秒経てばm_countは60になっているはず
		float frameCount = (float)(g_fps.m_count * 1000);
		// 上記数字÷経過時間で想定のFPSになる
		g_fps.m_drawFps = frameCount / difTime;
		// カウントは0に戻す
		g_fps.m_count = 0;
		// 現在時間と1秒前の時間を更新
		g_fps.m_prevDrawTime = g_fps.m_nowTime;
		
	}

	// FPSは常に表示を続ける
	DrawFormatString(16, 16, GetColor(255, 255, 255),
		"FPS:%.1lf", g_fps.m_drawFps);
}
