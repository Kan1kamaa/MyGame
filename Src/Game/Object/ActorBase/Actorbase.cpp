#include "ActorBase.h"

//アニメーションのタイプ
enum tagAnimState
{
	ANIMSTATE_NORMAL,	//通常再生　最後に停止
	ANIMSTATE_LOOP,		//ループ再生
};

//コンストラクタ
ActorBase::ActorBase() : m_animData{}
{

}

//デストラクタ
ActorBase::~ActorBase()
{
	Fin();
}

//初期化処理
void ActorBase::Init()
{
	ObjectBase::Init();
	m_animData = { 0.0f,0.0f,0.0f,-1,-1,0 };
}

//アニメーション再生(最終フレームで停止)
void ActorBase::RequestAnim(int index, float animSpeed, int animSrcHndl, bool isCheck)
{
	//同じアニメがすでに再生中なら何もしない(最初から再生し直さない)
	if (index == m_animData.m_animIndex)
	{
		return;
	}
	//アニメ消去
	DetachAnim();

	//アニメーション再生処理と各種データセット
	m_animData.m_attachID = MV1AttachAnim(m_modelHandle, index, animSrcHndl, isCheck);

	m_animData.m_animIndex = index;

	m_animData.m_currentFrame = 0.0f;

	m_animData.m_totalFrames = MV1GetAnimTotalTime(m_modelHandle, m_animData.m_animIndex);

	m_animData.m_speed = animSpeed;

	m_animData.m_playMode = ANIMSTATE_NORMAL;

}

//アニメーションループ再生
void ActorBase::RequestLoopAnim(int index, float animSpeed, int animSrcHndl, bool isCheck)
{
	RequestAnim(index, animSpeed, animSrcHndl, isCheck);
	m_animData.m_playMode = ANIMSTATE_LOOP;
}

//安全にアニメをデタッチ
void ActorBase::DetachAnim()
{
	if (m_animData.m_attachID != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animData.m_attachID);
		m_animData.m_attachID = -1;
	}
}

//更新処理
void ActorBase::Update()
{
	ObjectBase::Update();
	//アニメーションしてなかったら終了
	if (m_animData.m_attachID == -1)
	{
		return;
	}

	//アニメ時間更新
	m_animData.m_currentFrame += m_animData.m_speed;

	//最後まできたら状況に合わせて動作を変える
	if (m_animData.m_currentFrame >= m_animData.m_totalFrames)
	{
		switch (m_animData.m_playMode)
		{
		case ANIMSTATE_NORMAL:
			//最終フレームで停止
			m_animData.m_currentFrame = m_animData.m_totalFrames;
			break;

		case ANIMSTATE_LOOP:
			//最初のフレームに戻る
			m_animData.m_currentFrame = 0.0f;
			break;
		}
	}
	//アニメ時間を設定
	MV1SetAttachAnimTime(m_modelHandle, m_animData.m_attachID, m_animData.m_currentFrame);
}
