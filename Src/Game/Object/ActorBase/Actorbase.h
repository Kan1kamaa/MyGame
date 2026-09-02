#pragma once
#include "../ObjectBase/ObjectBase.h"

//アニメーションする物体の基底クラス
class ActorBase : public ObjectBase
{
protected:
	typedef struct
	{
		float m_nowFrm;		//現在のアニメ再生時間
		float m_endFrm;		//アニメの終了時間
		float m_speed;		//アニメーションの速度
		int	  m_attachID;	//アタッチID
		int   m_index;		//再生中のアニメID
		int   m_state;		//アニメの状態(ループor再生後停止)
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
	int GetAnimIndex() const { return m_animData.m_index; }
	int GetAnimAttachID() const { return m_animData.m_attachID; }

private:
	//安全にアニメをデタッチ
	void DetachAnim();
};
