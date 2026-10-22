#include"DebugCamera.h"
#include"math.h"

static const VECTOR VEC_ZERO = { 0.0f,0.0f,0.0f };
static const float CAMERA_NEAR = 1.0f;
static const float CAMERA_FAR = 1000.0f;

//コンストラクタ
DebugCamera::DebugCamera() : m_cameraPos(VEC_ZERO), m_focusPos(VEC_ZERO), m_upVec(VEC_ZERO),m_speed(VEC_ZERO),m_cameraRot(VEC_ZERO)
{
}

//デストラクタ
DebugCamera::~DebugCamera()
{
}

//初期化
void DebugCamera::Init()
{
	m_cameraPos = { 0,0,0 };
	m_focusPos = VEC_ZERO;
	m_upVec = { 0.0f,1.0f,0.0f };
	
}

void DebugCamera::Draw()

{
	DrawFormatString(16, 32, GetColor(255, 255, 255),
		"デバッグモード");
	DrawFormatString(16, 48, GetColor(255, 255, 255),
		"座標：%.0f,%.0f,%.0f", m_cameraPos.x,m_cameraPos.y,m_cameraPos.z);
	DrawFormatString(16, 64, GetColor(255, 255, 255),
		"角度：%.0f,%.0f,%.0f", m_cameraRot.x, m_cameraRot.y, m_cameraRot.z);
}
//更新
void DebugCamera::Step(VECTOR pos, float roty)
{
	VECTOR moveDir = VEC_ZERO;
	float moveSpeed = 0.0f;
	float moveAngle = 0.0f;
	if (CheckHitKey(KEY_INPUT_A))
	{
		moveSpeed = 1;
		moveAngle = m_cameraRot.y - DX_PI_F / 2;
	}
	if (CheckHitKey(KEY_INPUT_D))
	{
		moveSpeed = 1;
		moveAngle = m_cameraRot.y + DX_PI_F / 2;
	}
	if (CheckHitKey(KEY_INPUT_W))
	{
		moveSpeed = 1;
		moveAngle = m_cameraRot.y;
	}
	else if (CheckHitKey(KEY_INPUT_S))
	{
		moveSpeed = 1;
		moveAngle = m_cameraRot.y + DX_PI_F;
	}
	if (CheckHitKey(KEY_INPUT_UP))
	{
		m_cameraPos.y += 1;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN))
	{
		m_cameraPos.y -= 1;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		m_cameraRot.y += 0.05;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT))
	{
		m_cameraRot.y -= 0.05;
	}
	
	moveDir.x = sinf(moveAngle) * moveSpeed;
	moveDir.y = 0;
	moveDir.z = cosf(moveAngle) * moveSpeed;
	m_cameraPos = VAdd(m_cameraPos, moveDir);
}

void DebugCamera::Update()
{
	SetCameraPositionAndAngle(m_cameraPos, m_cameraRot.x,m_cameraRot.y,m_cameraRot.z );
}