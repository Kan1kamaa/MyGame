#include "FairyManager.h"

//キーが「今のフレームで押された瞬間」かどうかを返す。
//prev には前フレームの押下状態が入っていて、この関数の中で更新する。
bool IsPressedNow(int keyCode, bool& prev)
{
	//今のフレームで押されているか
	bool now = false;
	if (CheckHitKey(keyCode) != 0)
	{
		now = true;
	}

	//「今押されている」かつ「前のフレームでは押されていなかった」なら押した瞬間
	bool triggered = false;
	if (now == true)
	{
		if (prev == false)
		{
			triggered = true;
		}
	}

	//次のフレームのために、今回の状態を覚えておく
	prev = now;
	return triggered;
}

//コンストラクタ
FairyManager::FairyManager()
{
}

//デストラクタ
FairyManager::~FairyManager()
{
}

//初期化
void FairyManager::Init()
{
	m_nowFairy = nullptr;
	m_nowType = NONE;
	m_prevKeyE = false;
	m_prevKey1 = false;
	m_prevKey2 = false;
	m_prevKey3 = false;
}

//ロード
void FairyManager::Load()
{
}

void FairyManager::SwapFairy(FairyType nextType,VECTOR playerpos)
{
	if (m_nowType == nextType)return;
	if(m_nowType != NONE)
	{
		m_LivingFairy.push_back(m_nowFairy);
	}
	m_nowType = nextType;
	switch (nextType)
	{
	case FairyManager::WIND:
		m_nowFairy = new WindFairy();
		break;
	case FairyManager::FIRE:
		m_nowFairy = new FireFairy();
		break;
	case FairyManager::THUNDER:
		m_nowFairy = new ThunderFairy();
		break;
	}
	m_nowFairy->Init();
	m_nowFairy->SetPos(VAdd(FAIRY_POS, playerpos));
}

//毎フレーム計算する処理
void FairyManager::Step(const VECTOR& playerPos)
{
	if (m_nowFairy != nullptr)
	{
		m_nowFairy->Step(playerPos);
	}
	for (int i = 0;i < m_LivingFairy.size(); i++)
	{
		m_LivingFairy[i]->Step(playerPos);
	}
	bool isFairyWindTrigger = IsPressedNow(KEY_INPUT_1, m_prevKey1);
	if (isFairyWindTrigger == true)
	{
		SwapFairy(WIND,playerPos);
	}
	bool isFairyFireTrigger = IsPressedNow(KEY_INPUT_2, m_prevKey2);
	if (isFairyFireTrigger == true)
	{
		SwapFairy(FIRE, playerPos);
	}
	bool isFairyThunderTrigger = IsPressedNow(KEY_INPUT_3, m_prevKey3);
	if (isFairyThunderTrigger == true)
	{
		SwapFairy(THUNDER, playerPos);
	}
	bool isFairySkillTrigger = IsPressedNow(KEY_INPUT_E, m_prevKeyE);
	if (isFairySkillTrigger == true)
	{
		if (m_nowFairy != nullptr)
		{
			m_nowFairy->StartAttack();
		}
	}
}

//情報更新
void FairyManager::Update()
{
}

//描画
void FairyManager::Draw()
{
}

//破棄
void FairyManager::Fin()
{
}
