#include<math.h>
#include"Enemy.h"
#include"../../System/SoundManager.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };
static const float MOVE_RANGE = 300.0f;	//移動可能範囲(プレイヤーのMOVE_RANGE_X / MOVE_RANGE_Zと同じ)
static const float ENEMY_RAD = 5.0f;

//ランダム移動の調整用パラメータ
static const float ENEMY_MOVE_SPEED = 0.8f;	//1フレームあたりの移動量
static const int   DIR_CHANGE_MIN = 30;		//方向転換するまでの最短フレーム数
static const int   DIR_CHANGE_MAX = 90;		//方向転換するまでの最長フレーム数

//コンストラクタ
Enemy::Enemy() :m_speed(VEC_ZERO), m_changeDirCnt(0)
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
	m_status.Init(100,20);
	m_radius = ENEMY_RAD;
	m_speed = VEC_ZERO;
	m_changeDirCnt = 0;
	m_isActive = false;		//最初は見えないように消しておく
}

//移動方向をランダムに選び直す(XZ平面)
void Enemy::RandomizeDirection()
{
	//0〜359度のランダムな角度
	float rad = (float)(GetRand(359)) * DX_PI_F / 180.0f;
	m_speed.x = cosf(rad) * ENEMY_MOVE_SPEED;
	m_speed.y = 0.0f;
	m_speed.z = sinf(rad) * ENEMY_MOVE_SPEED;

	//次の方向転換までのフレーム数をランダムで決める
	m_changeDirCnt = DIR_CHANGE_MIN + GetRand(DIR_CHANGE_MAX - DIR_CHANGE_MIN);
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

	//カウントを減らし、0になったらランダムで新しい方向を選ぶ
	m_changeDirCnt--;
	if (m_changeDirCnt <= 0)
	{
		RandomizeDirection();
	}

	//現在の座標に速度を加算
	m_pos = VAdd(m_pos, m_speed);

	//範囲外に出たら消さずにフィールド内へ跳ね返す
	if (m_pos.x < -MOVE_RANGE) { m_pos.x = -MOVE_RANGE; m_speed.x = fabsf(m_speed.x); }
	if (m_pos.x >  MOVE_RANGE) { m_pos.x =  MOVE_RANGE; m_speed.x = -fabsf(m_speed.x); }
	if (m_pos.z < -MOVE_RANGE) { m_pos.z = -MOVE_RANGE; m_speed.z = fabsf(m_speed.z); }
	if (m_pos.z >  MOVE_RANGE) { m_pos.z =  MOVE_RANGE; m_speed.z = -fabsf(m_speed.z); }
}

//ショット発射
bool Enemy::Request(const VECTOR& pos, const VECTOR& speed)
{
	//既に発射されていたら終了
	if (m_isActive == true)return false;

	m_pos = pos;
	m_speed = speed;
	m_isActive = true;

	//すぐにランダムな方向へ動き出す
	RandomizeDirection();

	return true;
}

void Enemy::HitCalc(const ObjectBase& other)
{
	m_status.AddDamage()
	if (m_status.IsAlive() == false)
	{
		SoundManager::Play(SoundManager::SE_EXPLORE);
		m_isActive = false;
	}
}
