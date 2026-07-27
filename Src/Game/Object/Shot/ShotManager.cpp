#include"ShotManager.h"

//オリジナルとなる弾のパス
static const char FILE_PATH[] = "Data/models/Shot/Shot.mv1";

//コンストラクタ
ShotManager::ShotManager()
{
}

//デストラクタ
ShotManager::~ShotManager()
{
	Fin();
}

//初期化
void ShotManager::Init()
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Init();
	}
}

//ロード
void ShotManager::Load()
{
	//まずはオリジナルのモデルデータをロード
	int originhndl = MV1LoadModel(FILE_PATH);
	//複製していく
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Load(originhndl);
	}
	//複製が終わったら元データは削除
	MV1DeleteModel(originhndl);
}

void ShotManager::Step()
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Step();
	}
}

void ShotManager::Update()
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Update();
	}
}

void ShotManager::Draw()
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Draw();
	}
}

void ShotManager::Fin()
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		m_playerShot[i].Fin();
	}
}
bool ShotManager::RequestPlayerAttack(const VECTOR& pos, const VECTOR& speed)
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		//一発リクエストに成功したら終了
		if (m_playerShot[i].Request(pos, speed) == true)
		{
			return true;
		}
	}
	//ここまで来たらリクエストに失敗
	return false;
}
bool ShotManager::RequestPlayerShot(const VECTOR& pos, const VECTOR& speed)
{
	for (int i = 0; i < PL_SHOT_MAX; i++)
	{
		if (m_playerShot[i].Request(pos, speed) == true)
		{
			return true;
		}
	}
	return false;
}