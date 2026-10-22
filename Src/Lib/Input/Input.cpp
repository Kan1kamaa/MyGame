#include <DxLib.h>
#include"input.h"

unsigned int Input::m_currentKeys;
unsigned int Input::m_previousKeys;

//キー入力更新
void Input::UpdateKeyInput()
{
	//前回のデータ更新
	m_previousKeys = m_currentKeys;
	//一度データを0にする
	m_currentKeys = 0;

	//wを押した
	if (CheckHitKey(KEY_INPUT_W))
	{
		m_currentKeys |= KEY_UP;
	}
	//sを押した
	if (CheckHitKey(KEY_INPUT_S))
	{
		m_currentKeys |= KEY_DOWN;
	}
	//dを押した
	if (CheckHitKey(KEY_INPUT_D))
	{
		m_currentKeys |= KEY_RIGHT;
	}
	//aを押した
	if (CheckHitKey(KEY_INPUT_A))
	{
		m_currentKeys |= KEY_LEFT;
	}
	if (CheckHitKey(KEY_INPUT_LSHIFT))
	{
		m_currentKeys |= KEY_DASH;
	}
	//SPACEを押した
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		m_currentKeys |= KEY_JUMP;
	}
	if ((GetMouseInput() & MOUSE_INPUT_LEFT))
	{
		m_currentKeys |= KEY_ATTACK;
	}
	

}
//指定されたキーを入力しているか
bool Input::IsKeyInput(unsigned int key)
{
	if (m_currentKeys & key) return true;
	else return false;
}
//指定されたキーを入力しているか(トリガー判定)
bool Input::IsKeyInputTrg(unsigned int key)
{
	if (((m_currentKeys & key) != 0) && ((m_previousKeys & key) == 0))
	{
		return true;
	}
	else return false;
}