#pragma once
#include "../ObjectBase/ObjectBase.h"

//アニメーションする物体の基底クラス
class ActorBase : public ObjectBase
{
protected:
	typedef struct
	{
		float m_currentFrame;	//現在のアニメ再生位置(フレーム数)
		float m_totalFrames;	//アニメの総フレーム数(終了位置)
		float m_speed;			//1回のUpdateで進める再生位置(アニメーション速度)
		int	  m_attachID;		//アタッチID(DxLibが返すアニメ識別子。-1ならアニメなし)
		int   m_animIndex;		//再生中のアニメ番号(モデルに入っている番号。-1なら未再生)
		int   m_playMode;		//再生方法(ANIMSTATE_NORMAL=最後で停止 / ANIMSTATE_LOOP=繰り返し)
	}ANIM_DATA;

	ANIM_DATA m_animData;	//アニメーションデータ 一式

public:
	//コンストラクタ・デストラクタ
	ActorBase();
	virtual ~ActorBase();

	//初期化処理
	virtual void Init();

	//情報更新
	virtual void Update();

	//=========================
	//アニメーション関連
	//=========================
	//アニメーション再生（最終フレームで停止）
	//@index       : アタッチするアニメ番号
	//@animSpeed   : アニメ再生速度
	//@animSrcHndl : 別モデルのアニメを使う場合の元ハンドル
	//@isCheck	   : 別モデルのアニメを使う場合にフレーム名チェックするか

	void RequestAnim(int index, float animSpeed, int animSrcHndl = -1, bool isCheck = false);

	void RequestLoopAnim(int index, float animSpeed, int animSrcHndl = -1, bool isCheck = false);

	//デバッグ用:現在再生中のアニメ番号とアタッチ結果(-1ならアタッチ失敗)
	int GetAnimIndex() const { return m_animData.m_animIndex; }
	int GetAnimAttachID() const { return m_animData.m_attachID; }

private:
	//安全にアニメをデタッチ
	void DetachAnim();
};
