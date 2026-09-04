#include"MainCamera.h"
#include"math.h"
static const VECTOR VEC_ZERO = { 0.0f,0.0f,0.0f };
static const float CAMERA_NEAR = 1.0f;
static const float CAMERA_FAR = 1000.0f;

namespace {
	//SetGraphMode(Main.cpp)で指定している解像度の中心座標
	const int SCREEN_CENTER_X = 1280 / 2;
	const int SCREEN_CENTER_Y = 720 / 2;

	const float MOUSE_SENSITIVITY_YAW = 0.005f;		//マウス感度(左右)
	const float MOUSE_SENSITIVITY_PITCH = 0.003f;	//マウス感度(上下)
	const float CAMERA_PITCH_MIN = -0.2f;			//見上げすぎ防止
	const float CAMERA_PITCH_MAX = 1.3f;			//見下ろしすぎ防止

	const float CAMERA_DEFAULT_DISTANCE = 35.0f;	//注視点からの距離
	const float CAMERA_INITIAL_PITCH = 0.7f;		//カメラの高さ
	const float FOCUS_HEIGHT = 10.0f;				//注視点の高さ(キャラの足元からのオフセット)
}

//コンストラクタ
MainCamera::MainCamera() : m_camerapos(VEC_ZERO), m_focuspos(VEC_ZERO), m_upvec(VEC_ZERO),
m_yaw(0.0f), m_pitch(CAMERA_INITIAL_PITCH), m_distance(CAMERA_DEFAULT_DISTANCE)
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
	m_yaw = 0.0f;
	m_pitch = CAMERA_INITIAL_PITCH;
	m_distance = CAMERA_DEFAULT_DISTANCE;

	//カメラのニアーファー設定
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	//マウス操作の初期化(いきなり大きく視点が飛ばないよう中央に合わせておく)
	SetMouseDispFlag(FALSE);
	SetMousePoint(SCREEN_CENTER_X, SCREEN_CENTER_Y);
}

//マウスの動きでカメラの向きだけを更新する
void MainCamera::UpdateLook()
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int dx = mouseX - SCREEN_CENTER_X;
	int dy = mouseY - SCREEN_CENTER_Y;

	m_yaw += dx * MOUSE_SENSITIVITY_YAW;
	m_pitch -= dy * MOUSE_SENSITIVITY_PITCH;

	if (m_pitch < CAMERA_PITCH_MIN)
	{
		m_pitch = CAMERA_PITCH_MIN;
	}
	else if (m_pitch > CAMERA_PITCH_MAX)
	{
		m_pitch = CAMERA_PITCH_MAX;
	}

	//次のフレームでも差分を取れるようカーソルを中央に戻す
	SetMousePoint(SCREEN_CENTER_X, SCREEN_CENTER_Y);
}

//注視点を中心に、現在の向きに応じたカメラ座標を計算する
void MainCamera::Step(VECTOR targetPos)
{
	m_focuspos = targetPos;
	m_focuspos.y += FOCUS_HEIGHT;

	VECTOR offset;
	offset.x = sinf(m_yaw) * cosf(m_pitch) * m_distance;
	offset.y = sinf(m_pitch) * m_distance;
	offset.z = cosf(m_yaw) * cosf(m_pitch) * m_distance;

	m_camerapos = VAdd(m_focuspos, offset);
}

void MainCamera::Update()
{
	//カメラの情報をDxLibに反映
	SetCameraPositionAndTargetAndUpVec(m_camerapos, m_focuspos, m_upvec);
}
