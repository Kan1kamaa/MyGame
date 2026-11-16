#include "FairyBase.h"
#include "../../System/ModelUtil.h"
#include "math.h"

const float MOVE_SPEED = 0.1;			//精霊の移動速度
static const float ANIM_SPEED = 0.5f;	//アニメーションの速度
//--- スキル判定の可視化(デバッグ用) ---
//※色は GetColor を使うので、DXライブラリの初期化後(描画のとき)に作る
static const bool SKILL_DEBUG_DRAW = true;		//false にすると、判定の球を表示しない
//コンストラクタ
FairyBase::FairyBase()
{
	m_FlowTimer = 0;
	m_MoveSpeed = MOVE_SPEED;
	m_TargetPos = {0,0,0};
	m_state = FOLLOW;
	m_isLeaving = false;
	m_effectRequest = -1;
	m_hitCnt = 0;
	m_element = NONE;
	m_SkillCenter = { 0.0f,0.0f,0.0f };
	m_SkillRotY = 0.0f;
	m_SkillMinFrame = 0;
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
	m_effectRequest = -1;
	m_hitCnt = 0;
	m_SkillCenter = { 0.0f, 0.0f, 0.0f };
	m_SkillRotY = 0.0f;
	m_SkillMinFrame = 0;
}

//毎フレーム計算する処理
void FairyBase::Step(const VECTOR& playerPos,float rotY)
{
	VECTOR dis;
	m_FlowTimer++;
	switch (m_state)
	{
	case FairyBase::FOLLOW:
		if (m_isLeaving != true)
		{
			m_rot.y = rotY;
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
				RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
			}
			else
			{
				m_state = END;
			}
		}
		m_hitCnt++;
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

	//スキルの判定の可視化(デバッグ):スキル中は、判定の球をワイヤーフレームで表示する
	//ダメージが入るフレームだけ色を変える
	if (SKILL_DEBUG_DRAW == true && IsSkillActive() == true)
	{
		for (int k = 0; k < GetSkillHitNum(); k++)
		{
			unsigned int color = GetColor(80, 160, 255);		//スキル中の判定の球の色(青)
			if (IsSkillHitAt(k) == true)
			{
				color = GetColor(255, 80, 80);					//ダメージが入るフレームの色(赤)
			}
			DrawSphere3D(GetSkillHitPos(k), GetSkillHitRadius(k), 16, color, GetColor(0, 0, 0), FALSE);
		}
	}
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
	else if (m_hitCnt < m_SkillMinFrame)
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

bool FairyBase::PopEffectRequest(EffectManager::EffectId& id)
{
	if (m_effectRequest == -1)
	{
		return false;
	}
	id = (EffectManager::EffectId)m_effectRequest;
	m_effectRequest = -1;
	return true;
}
