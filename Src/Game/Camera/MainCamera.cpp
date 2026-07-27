#include"MainCamera.h"
#include"math.h"
static const VECTOR VEC_ZERO = { 0.0f,0.0f,0.0f };
static const float CAMERA_NEAR = 1.0f;
static const float CAMERA_FAR = 1000.0f;

//コンストラクタ
MainCamera::MainCamera() : m_camerapos(VEC_ZERO), m_focuspos(VEC_ZERO), m_upvec(VEC_ZERO)
{
}

//デストラクタ
MainCamera::~MainCamera()
{
}

//初期化
void MainCamera::Init()
{
	m_camerapos = VEC_ZERO;
	m_focuspos = VEC_ZERO;
	m_upvec = { 0.0f,1.0f,0.0f };
	//カメラのニアーファー設定
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);
}

//更新
void MainCamera::Step(VECTOR pos,float roty)
{
	VECTOR  camerarot = { 0.0f,0.0f,0.0f };
	camerarot.x = sinf(roty) * 30;
	camerarot.z = cosf(roty) * 30;
	m_camerapos = pos;
	m_camerapos = VAdd(m_camerapos, camerarot);
	m_focuspos = pos;
	m_camerapos.y += 28;
	m_focuspos.y += 10;
	
}

void MainCamera::Update()
{
	//カメラの情報をDxLibに反映
	SetCameraPositionAndTargetAndUpVec(m_camerapos, m_focuspos, m_upvec);
}