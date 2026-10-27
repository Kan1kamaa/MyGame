#include "FairyBase.h"
#include "math.h"

const float MOVE_SPEED = 0.1;			//精霊の移動速度
const VECTOR FAIRY_POS = { 0,50,50 };	//精霊がプレイヤーからどれだけ離れているか
//コンストラクタ
FairyBase::FairyBase()
{
	m_AttackCount = 0;
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
	m_AttackCount = 0;
	m_FlowTimer = 0;
	m_MoveSpeed = MOVE_SPEED;
	m_TargetPos = { 0,0,0 };
}

//毎フレーム計算する処理
void FairyBase::Step(const VECTOR& playerPos)
{
}

//情報更新
void FairyBase::Update()
{
}

//描画
void FairyBase::Draw()
{
}

//破棄
void FairyBase::Fin()
{
}