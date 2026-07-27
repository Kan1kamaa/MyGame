#include "SceneManager.h"
#include "../../Lib/Fade/Fade.h"
// コンストラクタ
SceneManager::SceneManager() : m_state(TITLE)
{
	// 提出時はMAINからTITLEに変える
	FADE::Init();
	//ゲーム起動時にサウンドの初期化＆ロード
	SoundManager::Init();
	SoundManager::Load();
}

//デストラクタ
SceneManager::~SceneManager()
{
	SoundManager::Exit();
}
// ゲームの計算もろもろ 
void SceneManager::Loop()
{
	int ret = 0;

	switch (m_state) {
	case TITLE:
		ret = m_title.Loop();
		if (ret != -1)
		{
			m_state = MAIN;
		}
		break;
	case MAIN:
		ret = m_play.Loop();
		// ゲームが終了した場合、戻り値は-1以外
		if (ret != -1)
		{
			m_state = RESULT;
		}
		break;
	case RESULT:
		ret = m_result.Loop();
		// ゲームが終了した場合、戻り値は-1以外
		if (ret != -1)
		{
			m_state = TITLE;
		}
		break;
	}
	FADE::Update();
}

// 描画全般
void SceneManager::Draw()
{
	switch (m_state) {
	case TITLE:
		m_title.Draw();
		break;
	case MAIN:
		m_play.Draw();
		break;
	case RESULT:
		m_result.Draw();
		break;
	}
	FADE::Draw();
}