#include "FairyBase.h"
#include "../../System/ModelUtil.h"
#include "math.h"

const float MOVE_SPEED = 0.1;			//精霊の移動速度
static const float ANIM_SPEED = 0.5f;	//アニメーションの速度
//コンストラクタ
FairyBase::FairyBase()
{
	m_FlowTimer = 0;
	m_MoveSpeed = MOVE_SPEED;
	m_TargetPos = {0,0,0};
	m_state = FOLLOW;
	m_isLeaving = false;
}

//デストラクタ
FairyBase::~FairyBase()
{
}

//初期化
void FairyBase::Init()
{
	ActorBase::Init();
	m_state = FOLLOW;
	m_FlowTimer = 0;
	m_MoveSpeed = MOVE_SPEED;
	m_TargetPos = { 0,0,0 };
	m_isLeaving = false;
}

//毎フレーム計算する処理
void FairyBase::Step(const VECTOR& playerPos)
{
	VECTOR dis;
	m_FlowTimer++;
	switch (m_state)
	{
	case FairyBase::FOLLOW:
		if (m_isLeaving != true)
		{
			m_TargetPos = VAdd(playerPos, FAIRY_POS);
			dis = VSub(m_TargetPos, m_pos);
			m_pos = VAdd(m_pos, VScale(dis, m_MoveSpeed));
		}
		break;
	case FairyBase::ATTACK:
		break;
	case FairyBase::SKILL:
		if (IsSkillFinished() == true)
		{
			if (m_isLeaving != true)
			{
				m_state = FOLLOW;
			}
			else
			{
				m_state = END;
			}
		}
		break;
	case FairyBase::END:
		break;
	}
}

//情報更新
void FairyBase::Update()
{
	ActorBase::Update();
}

//描画
void FairyBase::Draw()
{
	ObjectBase::Draw();
}

//破棄
void FairyBase::Fin()
{
	ObjectBase::Fin();
}

//モデルを持たせて、待機アニメをループ再生する
void FairyBase::SetModel(int hndl)
{
	m_hndl = hndl;
	if (m_hndl == -1)
	{
		return;
	}
	BrightenModelAmbient(m_hndl);
	//アニメが入っているモデルのときだけ再生する
	if (MV1GetAnimNum(m_hndl) > ANIMID_IDLE)
	{
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
	}
}

//スキルのモーションを最後まで再生し終えたか
bool FairyBase::IsSkillFinished() const
{
	if (m_state != SKILL)
	{
		return false;
	}
	if (m_animData.m_nowFrm >= m_animData.m_endFrm)
	{
		return true;
	}
	return false;
}

void FairyBase::isLeave()
{
	m_isLeaving = true;
	if (m_state == FOLLOW)
	{
		m_state = END;
	}
}

bool FairyBase::isEnd()const
{
	if (m_state == END)
	{
		return true;
	}
	else
	{
		return false;
	}
}