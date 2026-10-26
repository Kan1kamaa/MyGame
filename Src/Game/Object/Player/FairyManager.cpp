#include "FairyManager.h"
#include "WindFairy.h"
#include "FireFairy.h"
#include "ThunderFairy.h"

//追従位置の候補(プレイヤーからの相対座標)
static const VECTOR FAIRY_OFFSETS[] = {
	{  60.0f, 150.0f, -60.0f },	//右後ろ
	{ -60.0f, 150.0f, -60.0f },	//左後ろ
	{   0.0f, 200.0f,   0.0f },	//頭上
	{   0.0f, 130.0f,  80.0f },	//前方
};
static const int FAIRY_OFFSET_NUM = sizeof(FAIRY_OFFSETS) / sizeof(FAIRY_OFFSETS[0]);

FairyManager::FairyManager()
	: m_active(nullptr)
	, m_element(FairyElement::Wind)
	, m_offsetIdx(0)
	, m_prevKeyE(false)
	, m_prevKeyR(false)
	, m_prevKey1(false)
	, m_prevKey2(false)
	, m_prevKey3(false)
{
}

FairyManager::~FairyManager()
{
}

void FairyManager::Init(VECTOR playerPos)
{
	m_element = FairyElement::Wind;
	m_offsetIdx = 0;
	m_active = CreateFairy(m_element, VAdd(playerPos, GetOffset()));
}

void FairyManager::Step(VECTOR playerPos)
{
	ReadInput();

	if (m_active) m_active->Step(playerPos);

	//切り替え済みの精霊は攻撃を出し切ったら削除する
	for (size_t i = 0; i < m_leaving.size(); ) {
		m_leaving[i]->Step(playerPos);
		if (m_leaving[i]->IsFinished()) {
			DestroyFairy(m_leaving[i]);
			m_leaving.erase(m_leaving.begin() + i);
		}
		else {
			i++;
		}
	}
}

void FairyManager::Update()
{
	if (m_active) m_active->Update();
	for (FairyBase* fairy : m_leaving) fairy->Update();
}

void FairyManager::DrawFairy()
{
	if (m_active) m_active->Draw();
	for (FairyBase* fairy : m_leaving) fairy->Draw();
}

void FairyManager::Fin()
{
	if (m_active) {
		DestroyFairy(m_active);
		m_active = nullptr;
	}
	for (FairyBase* fairy : m_leaving) DestroyFairy(fairy);
	m_leaving.clear();
}

void FairyManager::Reset(VECTOR playerPos)
{
	Fin();
	Init(playerPos);
}

int FairyManager::GetFairyCount() const
{
	return (int)m_leaving.size() + (m_active ? 1 : 0);
}

FairyBase* FairyManager::GetFairy(int index) const
{
	if (m_active) {
		if (index == 0) return m_active;
		index--;
	}
	if (index < 0 || index >= (int)m_leaving.size()) return nullptr;
	return m_leaving[index];
}

void FairyManager::ReadInput()
{
	bool keyE = CheckHitKey(KEY_INPUT_E) != 0;
	bool keyR = CheckHitKey(KEY_INPUT_R) != 0;
	bool key1 = CheckHitKey(KEY_INPUT_1) != 0;
	bool key2 = CheckHitKey(KEY_INPUT_2) != 0;
	bool key3 = CheckHitKey(KEY_INPUT_3) != 0;

	//押した瞬間だけ反応させる
	if (keyE && !m_prevKeyE && m_active) m_active->StartAttack();
	if (keyR && !m_prevKeyR) CycleOffset();
	if (key1 && !m_prevKey1) SwapElement(FairyElement::Wind);
	if (key2 && !m_prevKey2) SwapElement(FairyElement::Fire);
	if (key3 && !m_prevKey3) SwapElement(FairyElement::Thunder);

	m_prevKeyE = keyE;
	m_prevKeyR = keyR;
	m_prevKey1 = key1;
	m_prevKey2 = key2;
	m_prevKey3 = key3;
}

void FairyManager::SwapElement(FairyElement element)
{
	//同じ属性なら何もしない
	if (m_active && element == m_element) return;

	VECTOR startPos = GetOffset();
	if (m_active) {
		startPos = m_active->GetPos();
		//今の精霊はその場に残して攻撃を出し切らせる
		m_active->Leave();
		m_leaving.push_back(m_active);
		//残る数が上限を超えたら一番古いものを消す
		if ((int)m_leaving.size() > MAX_LEAVING) {
			DestroyFairy(m_leaving.front());
			m_leaving.erase(m_leaving.begin());
		}
	}

	m_element = element;
	m_active = CreateFairy(element, startPos);
}

void FairyManager::CycleOffset()
{
	m_offsetIdx = (m_offsetIdx + 1) % FAIRY_OFFSET_NUM;
	if (m_active) m_active->SetFollowOffset(GetOffset());
}

FairyBase* FairyManager::CreateFairy(FairyElement element, VECTOR startPos)
{
	FairyBase* fairy = nullptr;
	switch (element) {
	case FairyElement::Wind:    fairy = new WindFairy();    break;
	case FairyElement::Fire:    fairy = new FireFairy();    break;
	case FairyElement::Thunder: fairy = new ThunderFairy(); break;
	}
	if (!fairy) return nullptr;

	fairy->Init();
	fairy->SetPos(startPos);
	fairy->SetFollowOffset(GetOffset());
	return fairy;
}

void FairyManager::DestroyFairy(FairyBase* fairy)
{
	if (!fairy) return;
	fairy->Fin();
	delete fairy;
}

VECTOR FairyManager::GetOffset() const
{
	return FAIRY_OFFSETS[m_offsetIdx];
}
