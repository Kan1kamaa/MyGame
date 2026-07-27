#include "Scenebase.h"
#include "../../Lib/Fade/Fade.h"
SceneBase::SceneBase():m_state(INIT),m_nextScene(0)
{
}


int SceneBase::Loop()
{
	int result = -1;

	switch (m_state)
	{
	case SceneBase::INIT:
		Init();
		break;
	case SceneBase::LOAD:
		Load();
		FADE::RequestFadeIn();
		break;
	case SceneBase::START:
		if (FADE::IsEndFadeIn())
			m_state = MAIN;
		break;
	case SceneBase::MAIN:
		Step();
		Update();
		if (m_state == END_WAIT)
			FADE::RequestFadeOut();
		break;
	case SceneBase::END_WAIT:
		if (FADE::IsEndFadeOut())
			m_state = END;
	case SceneBase::END:
		Fin();
		//ここまで来たら終わりなので、戻り値を変更
		//各シーンのどこかでm_nextSceneを変更してもらう
		result = m_nextScene;
		break;
	}
	return result;
}