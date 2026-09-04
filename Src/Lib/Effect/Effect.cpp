#include"Effect.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };
static const float MOVE_RANGE = 2000.0f;	//移動可能範囲
static const float SHOT_RAD = 5.0f;
//コンストラクタ
Effect::Effect() :m_speed(VEC_ZERO)
{
}

//デストラクタ
Effect::~Effect()
{
	Fin();
}

//初期化
void Effect::Init()
{
	ObjectBase::Init();
	m_radius = SHOT_RAD;
	m_speed = VEC_ZERO;
	m_isActive = false;		//最初は見えないように消しておく
}

//ロード
void Effect::Load(int origiinhndl)
{
	if (m_hndl == -1)
	{
		//モデルは複製する
		m_hndl = MV1DuplicateModel(origiinhndl);
	}
}

//毎フレーム計算する処理
void Effect::Step()
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

//エフェクト出現
bool Effect::Request(const VECTOR& pos, const VECTOR& speed)
{
	//既に発射されていたら終了
	if (m_isActive == true)return false;

	m_pos = pos;
	m_speed = speed;
	m_isActive = true;
	return true;
}