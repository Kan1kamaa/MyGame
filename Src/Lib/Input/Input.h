#pragma once
//使用するボタンにビットを割り当てる
#define KEY_UP		(0b1)				//上
#define KEY_DOWN	(0b10)				//下
#define KEY_RIGHT	(0b100)				//右
#define KEY_LEFT	(0b1000)			//左
#define KEY_DASH	(0b10000)			//ダッシュ
#define KEY_JUMP	(0b100000)			//ジャンプ
#define KEY_ATTACK	(0b1000000)			//アタック
//入力クラス
class Input {
private:
	static unsigned int m_nowkey;	//今回入力したキー
	static unsigned int m_prekey;	//前回入力したキー
public:
	//キー入力判定(通常判定)
	static bool IsKeyInput(unsigned int key);
	//キー入力判定(トリガー判定)
	static bool IsKeyInputTrg(unsigned int key);
	//キー入力更新
	void UpdataKeyInput();

};
