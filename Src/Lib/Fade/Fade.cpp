#include <DxLib.h>
#include"Fade.h"
#define WINDOW_SIZE_X (1280)	// ウィンドウサイズ(横)
#define WINDOW_SIZE_Y (720)	// ウィンドウサイズ(縦)
#define FADE_SPEED (10)		// フェード速度

FADE::tagFade FADE::m_state;
int FADE::m_count;

//-------------------------------
//		フェード初期化
//-------------------------------
void FADE::Init()
{
	// 最初はひとまず画面が表示されるNONに
	m_state = FADE_NON;
	m_count = 0;
}


//-------------------------------
//		フェード更新
//-------------------------------
void FADE::Update()
{
	switch (m_state)
	{
		// 徐々に明るくしていく
	case FADE_IN:
		m_count -= FADE_SPEED;
		if (m_count <= 0)
		{
			m_count = 0;
			m_state = FADE_NON;
		}
		break;
		// 徐々に暗くしていく
	case FADE_OUT:
		m_count += FADE_SPEED;
		if (m_count >= 255)
		{
			m_count = 255;
			m_state = FADE_OUT_WAIT;
		}
		break;
	}
}


//-------------------------------
//		フェード描画
//-------------------------------
void FADE::Draw()
{
	// FADE_NONの時だけは何も表示しなくてよい
	switch (m_state)
	{
	case FADE_IN:
	case FADE_OUT:
	case FADE_OUT_WAIT:
		// ここでアルファ値をセットする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_count);

		// フェード用の黒い四角を表示
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y,
			GetColor(0, 0, 0), TRUE);

		// 他に影響を及ぼさないように、アルファ値設定を無効化
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		break;
	}
}


//-------------------------------
//		フェードインリクエスト
//-------------------------------
void FADE::RequestFadeIn()
{
	// 状態設定と画面を見えなくするための設定
	m_state = FADE_IN;
	m_count = 255;
}


//-------------------------------
//		フェードアウトリクエスト
//-------------------------------
void FADE::RequestFadeOut()
{
	// 状態設定と画面を見えるようにするための設定
	m_state = FADE_OUT;
	m_count = 0;
}


//-------------------------------
//		フェードインが終了したか？
//-------------------------------
bool FADE::IsEndFadeIn()
{
	// フェードイン状態以外は終了と判断
	if (m_state == FADE_IN) return false;
	else return true;
}


//-------------------------------
//		フェードアウトが終了したか？
//-------------------------------
bool FADE::IsEndFadeOut()
{
	// フェードアウト状態以外は終了と判断
	if (m_state == FADE_OUT) return false;
	else return true;
}

