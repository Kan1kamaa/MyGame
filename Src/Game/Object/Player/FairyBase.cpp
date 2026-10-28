#include "FairyBase.h"
#include "math.h"

const float MOVE_SPEED = 0.1;			//精霊の移動速度
//コンストラクタ
FairyBase::FairyBase()
{
	m_FlowTimer = 0;
	m_MoveSpeed = MOVE_SPEED;
	m_TargetPos = {0,0,0};
	m_state = FOLLOW;
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
}

//毎フレーム計算する処理
void FairyBase::Step(const VECTOR& playerPos)
{
	VECTOR dis;
	m_FlowTimer++;
	switch (m_state)
	{
	case FairyBase::FOLLOW:
		m_TargetPos = VAdd(playerPos, FAIRY_POS);
		dis = VSub(m_TargetPos, m_pos);
		m_pos = VAdd(m_pos, VScale(dis, m_MoveSpeed));
		break;
	case FairyBase::ATTACK:
		break;
	case FairyBase::SKILL:
		if (IsSkillFinished() == true)
		{
			m_state = END;
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