#include"Shot.h"
#include"../../System/SoundManager.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };
static const float MOVE_RANGE = 2000.0f;	//移動可能範囲
static const float SHOT_RAD = 5.0f;
//コンストラクタ
Shot::Shot() :m_speed(VEC_ZERO)
{
}

//デストラクタ
Shot::~Shot()
{
	Fin();
}

//初期化
void Shot::Init()
{
	ObjectBase::Init();
	m_radius = SHOT_RAD;
	m_speed = VEC_ZERO;
	m_isActive = false;		//最初は見えないように消しておく
}

//ロード
void Shot::Load(int origiinhndl)
{
	if (m_hndl == -1)
	{
		//モデルは複製する
		m_hndl = MV1DuplicateModel(origiinhndl);
	}
}

//毎フレーム計算する処理
void Shot::Step()
{
	//フラグオフなら終了
	if (m_isActive == false)return;

	//現在の座標に速度を計算
	m_pos = VAdd(m_pos, m_speed);
	//一定範囲を超えたら消す
	if (m_pos.x < -MOVE_RANGE || m_pos.x > MOVE_RANGE
		|| m_pos.z < -MOVE_RANGE || m_pos.z > MOVE_RANGE)
	{
		m_isActive = false;
	}
}

//ショット発射
bool Shot::Request(const VECTOR& pos, const VECTOR& speed)
{
	//既に発射されていたら終了
	if (m_isActive == true)return false;

	m_pos = pos;
	m_speed = speed;
	m_isActive = true;
	SoundManager::Play(SoundManager::SE_PLSHOT);
	return true;
}