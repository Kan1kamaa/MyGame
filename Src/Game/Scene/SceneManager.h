#pragma once
#include "TitleScene.h"
#include "PlayScene.h"
#include "resultScene.h"
// 各種シーン管理クラス
class SceneManager {
private:
	enum tagState {
		TITLE,	// タイトル画面
		MAIN,	// ゲーム本編
		RESULT,	// リザルト画面
	};

	TitleScene m_title;	// タイトル画面
	PlayScene m_play;	// ゲーム本編
	ResultScene m_result; //リザルト画面
	tagState m_state;	// 今どのシーンか

public:
	// コンストラクタ
	SceneManager();
	//デストラクタ
	~SceneManager();
	// ゲームの計算もろもろ 
	void Loop();
	// 描画全般
	void Draw();
};


