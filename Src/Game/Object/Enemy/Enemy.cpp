#include"Enemy.h"
#include"../../System/SoundManager.h"
#include <math.h>
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };
static const float MOVE_RANGE = 500.0f;	//移動可能範囲
static const float ENEMY_RAD = 5.0f;
static const int DIR_CHANGE_INTERVAL = 60;		// base frames between direction changes
static const int DIR_CHANGE_RAND_RANGE = 60;	// extra random frames added to the interval

//コンストラクタ
Enemy::Enemy() :m_speed(VEC_ZERO), m_speedMag(0.0f), m_dirChangeCnt(0)
{
}

//デストラクタ
Enemy::~Enemy()
{
	Fin();
}

//初期化
void Enemy::Init()
{
	ObjectBase::Init();
	m_radius = ENEMY_RAD;
	m_speed = VEC_ZERO;
	m_speedMag = 0.0f;
	m_dirChangeCnt = 0;
	m_isActive = false;		//最初は見えないように消しておく
}

//ロード
void Enemy::Load(int origiinhndl)
{
	if (m_hndl == -1)
	{
		//モデルは複製する
		m_hndl = MV1DuplicateModel(origiinhndl);
	}
}

//毎フレーム計算する処理
void Enemy::Step()
{
	//フラグオフなら終了
	if (m_isActive == false)return;

	//direction-change countdown: pick a new random direction when it reaches 0
	if (m_dirChangeCnt > 0)
	{
		m_dirChangeCnt--;
	}
	else
	{
		ChangeDirRandom();
	}

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
bool Enemy::Request(const VECTOR& pos, const VECTOR& speed)
{
	//既に発射されていたら終了
	if (m_isActive == true)return false;

	m_pos = pos;
	m_speed = speed;
	m_speedMag = sqrtf(speed.x * speed.x + speed.z * speed.z);
	m_dirChangeCnt = DIR_CHANGE_INTERVAL + GetRand(DIR_CHANGE_RAND_RANGE);
	m_isActive = true;

	return true;
}

void Enemy::HitCalc(const ObjectBase& other)
{
	SoundManager::Play(SoundManager::SE_EXPLORE);
	m_isActive = false;
}

//pick a new random movement direction, keeping the same speed magnitude
void Enemy::ChangeDirRandom()
{
	float angle = (float)GetRand(359) * (3.14159265f / 180.0f);
	m_speed.x = sinf(angle) * m_speedMag;
	m_speed.z = cosf(angle) * m_speedMag;

	//randomize the next interval too, so enemies do not all turn in sync
	m_dirChangeCnt = DIR_CHANGE_INTERVAL + GetRand(DIR_CHANGE_RAND_RANGE);
}
