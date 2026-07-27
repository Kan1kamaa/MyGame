#include"DebugCamera.h"
#include"math.h"

static const VECTOR VEC_ZERO = { 0.0f,0.0f,0.0f };
static const float CAMERA_NEAR = 1.0f;
static const float CAMERA_FAR = 1000.0f;

//コンストラクタ
DebugCamera::DebugCamera() : m_camerapos(VEC_ZERO), m_focuspos(VEC_ZERO), m_upvec(VEC_ZERO),m_speed(VEC_ZERO),m_camerarot(VEC_ZERO)
{
}

//デストラクタ
DebugCamera::~DebugCamera()
{
}

//初期化
void DebugCamera::Init()
{
	m_camerapos = { 0,0,0 };
	m_focuspos = VEC_ZERO;
	m_upvec = { 0.0f,1.0f,0.0f };
	
}

void DebugCamera::Draw()

{
	DrawFormatString(16, 32, GetColor(255, 255, 255),
		"デバッグモード");
	DrawFormatString(16, 48, GetColor(255, 255, 255),
		"座標：%.0f,%.0f,%.0f", m_camerapos.x,m_camerapos.y,m_camerapos.z);
	DrawFormatString(16, 64, GetColor(255, 255, 255),
		"角度：%.0f,%.0f,%.0f", m_camerarot.x, m_camerarot.y, m_camerarot.z);
}
//更新
void DebugCamera::Step(VECTOR pos, float roty)
{
	VECTOR dir = VEC_ZERO;
	float speed = 0.0f;
	float rot = 0.0f;
	if (CheckHitKey(KEY_INPUT_A))
	{
		speed = 1;
		rot = m_camerarot.y - DX_PI_F / 2;
	}
	if (CheckHitKey(KEY_INPUT_D))
	{
		speed = 1;
		rot = m_camerarot.y + DX_PI_F / 2;
	}
	if (CheckHitKey(KEY_INPUT_W))
	{
		speed = 1;
		rot = m_camerarot.y;
	}
	else if (CheckHitKey(KEY_INPUT_S))
	{
		speed = 1;
		rot = m_camerarot.y + DX_PI_F;
	}
	if (CheckHitKey(KEY_INPUT_UP))
	{
		m_camerapos.y += 1;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN))
	{
		m_camerapos.y -= 1;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT))
	{
		m_camerarot.y += 0.05;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT))
	{
		m_camerarot.y -= 0.05;
	}
	
	dir.x = sinf(rot) * speed;
	dir.y = 0;
	dir.z = cosf(rot) * speed;
	m_camerapos = VAdd(m_camerapos, dir);
}

void DebugCamera::Update()
{
	SetCameraPositionAndAngle(m_camerapos, m_camerarot.x,m_camerarot.y,m_camerarot.z );
}