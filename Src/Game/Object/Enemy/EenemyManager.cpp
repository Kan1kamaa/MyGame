#include"EnemyManager.h"

//オリジナルとなる敵のパス
static const char FILE_PATH[] = "Data/models/Enemy/Enemy.pmx";

static const int WAIT_COUNT(60);
//同時に出現させておく敵の数
static const int ACTIVE_ENEMY_MAX = 10;
//コンストラクタ
EnemyManager::EnemyManager()
{
}

//デストラクタ
EnemyManager::~EnemyManager()
{
	Fin();
}

//初期化
void EnemyManager::Init()
{
	m_waitCnt = WAIT_COUNT;
	m_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Init();
	}
}

//ロード
void EnemyManager::Load()
{
	//まずはオリジナルのモデルデータをロード
	int originhndl = MV1LoadModel(FILE_PATH);
	//複製していく
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Load(originhndl);
	}
	//複製が終わったら元データは削除
	MV1DeleteModel(originhndl);
}

void EnemyManager::Step()
{
	//毎フレーム現在の敵の数を数え直す
	m_EnemyCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Step();
		if (m_Enemy[i].GetActive() == true)
		{
			m_EnemyCnt++;
		}
	}

	//待機時間が過ぎていて、かつ場の敵が上限未満なら次の敵を出現させる
	if (m_waitCnt <= 0 && m_EnemyCnt < ACTIVE_ENEMY_MAX)
	{
		RequestEnemy();
		m_waitCnt = WAIT_COUNT;
	}
	m_waitCnt--;
}

void EnemyManager::Update()
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Update();
	}
}

void EnemyManager::Draw()
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Draw();
	}
}

void EnemyManager::Fin()
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Fin();
	}
}

bool EnemyManager::RequestEnemy()
{
	//フィールド内のランダムな位置に出現させる(移動方向はEnemy側でランダムに決まる)
	VECTOR pos = { (float)GetRand(500) - 250.0f,0.0f,(float)GetRand(500) - 250.0f };
	VECTOR speed = { 0.0f,0.0f,0.0f };
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (m_Enemy[i].Request(pos, speed) == true)
		{
			return true;
		}
	}
	return false;


}
