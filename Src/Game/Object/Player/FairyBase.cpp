#include "FairyBase.h"
#include "math.h"

//これ以上プレイヤーから離れたら補間せず瞬間移動する距離
static const float FAIRY_TELEPORT_DIST = 1000.0f;
//ふわふわ揺れる幅と速さ
static const float FAIRY_FLOAT_HEIGHT = 10.0f;
static const float FAIRY_FLOAT_SPEED = 0.05f;

FairyBase::FairyBase()
	: m_element(FairyElement::Wind)
	, m_state(FairyState::Summon)
	, m_stateCnt(0)
	, m_followOffset(VGet(60.0f, 150.0f, -60.0f))
	, m_followSpeed(0.1f)
	, m_floatTimer(0.0f)
	, m_isAttacking(false)
	, m_attackPower(0.0f)
{
}

FairyBase::~FairyBase()
{
}

void FairyBase::Init()
{
	ActorBase::Init();
	m_state = FairyState::Summon;
	m_stateCnt = 0;
	m_floatTimer = 0.0f;
	m_isAttacking = false;
}

void FairyBase::Step(VECTOR playerPos)
{
	m_stateCnt++;
	m_floatTimer += FAIRY_FLOAT_SPEED;

	switch (m_state) {
	case FairyState::Summon:
		FollowPlayer(playerPos);
		ChangeState(FairyState::Follow);
		break;
	case FairyState::Follow:
		FollowPlayer(playerPos);
		break;
	case FairyState::Attack:
		FollowPlayer(playerPos);
		StepAttack();
		if (!m_isAttacking) ChangeState(FairyState::Follow);
		break;
	case FairyState::Leaving:
		//追従しない。その場で攻撃を出し切る
		StepAttack();
		if (!m_isAttacking) ChangeState(FairyState::Vanish);
		break;
	case FairyState::Vanish:
		m_isActive = false;
		break;
	}
}

void FairyBase::Update()
{
	ActorBase::Update();
}

void FairyBase::Draw()
{
	if (!m_isActive) return;
	MV1DrawModel(m_hndl);
}

void FairyBase::Fin()
{
	if (m_hndl != -1) {
		MV1DeleteModel(m_hndl);
		m_hndl = -1;
	}
	m_isActive = false;
}

void FairyBase::Leave()
{
	ChangeState(FairyState::Leaving);
}

bool FairyBase::IsFinished() const
{
	return m_state == FairyState::Vanish;
}

void FairyBase::ChangeState(FairyState next)
{
	m_state = next;
	m_stateCnt = 0;
}

void FairyBase::FollowPlayer(VECTOR playerPos)
{
	VECTOR target = VAdd(playerPos, m_followOffset);
	target.y += sinf(m_floatTimer) * FAIRY_FLOAT_HEIGHT;

	//離れすぎ(リスポーンなど)なら補間せずに移動する
	if (VSize(VSub(target, m_pos)) > FAIRY_TELEPORT_DIST) {
		m_pos = target;
		return;
	}
	m_pos = VAdd(m_pos, VScale(VSub(target, m_pos), m_followSpeed));
}
