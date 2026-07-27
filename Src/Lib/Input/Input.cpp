#include <DxLib.h>
#include"input.h"

unsigned int Input::m_nowkey;
unsigned int Input::m_prekey;

//キー入力更新
void Input::UpdataKeyInput()
{
	//前回のデータ更新
	m_prekey = m_nowkey;
	//一度データを0にする
	m_nowkey = 0;

	//wを押した
	if (CheckHitKey(KEY_INPUT_W))
	{
		m_nowkey |= KEY_UP;
	}
	//aを押した
	if (CheckHitKey(KEY_INPUT_S))
	{
		m_nowkey |= KEY_DOWN;
	}
	//dを押した
	if (CheckHitKey(KEY_INPUT_D))
	{
		m_nowkey |= KEY_RIGHT;
	}
	//sを押した
	if (CheckHitKey(KEY_INPUT_A))
	{
		m_nowkey |= KEY_LEFT;
	}
	if (CheckHitKey(KEY_INPUT_LSHIFT))
	{
		m_nowkey |= KEY_DASH;
	}
	//SPACEを押した
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		m_nowkey |= KEY_JUMP;
	}
	if ((GetMouseInput() & MOUSE_INPUT_LEFT))
	{
		m_nowkey |= KEY_ATTACK;
	}
	

}
//指定されたキーを入力しているか
bool Input::IsKeyInput(unsigned int key)
{
	if (m_nowkey & key) return true;
	else return false;
}
//指定されたキーを入力しているか(トリガー判定)
bool Input::IsKeyInputTrg(unsigned int key)
{
	if (((m_nowkey & key) != 0) && ((m_prekey & key) == 0))
	{
		return true;
	}
	else return false;
}