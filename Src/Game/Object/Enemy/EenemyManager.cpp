#include"EnemyManager.h"

//オリジナルとなる敵のパス
static const char FILE_PATH[] = "Data/models/Enemy/Enemy.pmx";

static const int WAIT_COUNT(60);
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
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		m_Enemy[i].Step();
		if (m_Enemy[i].GetActive() == true)
		{
			m_EnemyCnt++;
		}
		//敵の出現を一旦止めている。再開する時はこのif文の中のコメントを外す
		if (m_waitCnt <= 0 && m_EnemyCnt <= 0)
		{
			RequestEnemy();
			m_waitCnt = WAIT_COUNT;
		}
		m_waitCnt--;
	}
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
	VECTOR pos = { (float)GetRand(200) - 100.0f,0.0f,200.0f };
	VECTOR speed = { 0.0f,0.0f,-0.5f };
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (m_Enemy[i].Request(pos, speed) == true)
		{
			return true;
		}
	}
	return false;


}
