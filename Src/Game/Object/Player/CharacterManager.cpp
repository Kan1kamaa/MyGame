#include "CharacterManager.h"
#include "math.h"

namespace {
	const float ROT_SPEED = 0.05f;	  //回転速度
	const float MOVE_SPEED = 1.0f;	  //移動速度
	const float MOVE_RANGE_X = 300.0f;    //移動範囲を制限
	const float MOVE_RANGE_Z = 300.0f;    //移動範囲を制限
}

//----------------------
//	コンストラクタ
//----------------------
CharacterManager::CharacterManager() : m_characters{ &m_char1, &m_char2 }, m_activeIndex(0)
{
}

//----------------------
//	デストラクタ
//----------------------
CharacterManager::~CharacterManager()
{
	Fin();
}

//----------------------
//	初期化処理
//----------------------
void CharacterManager::Init()
{
	ObjectBase::Init();
	m_radius = 5.0f;
	m_activeIndex = 0;
	m_char1.Init();
	m_char2.Init();
}

//----------------------
//	データロード
//----------------------
void CharacterManager::Load()
{
	m_char1.Load();
	m_char2.Load();
}

//----------------------
//	操作キャラクターの切り替え
//----------------------
void CharacterManager::SwitchActive(int index)
{
	m_activeIndex = index;
	//前のキャラの攻撃モーションのまま固まって見えないように待機に戻す
	m_characters[m_activeIndex]->ResetToIdle();
}

//----------------------
//	毎フレーム計算する処理
//----------------------
void CharacterManager::Step(ShotManager& shotManager)
{
	//操作キャラクターの切り替え
	if (CheckHitKey(KEY_INPUT_1) && m_activeIndex != 0)
	{
		SwitchActive(0);
	}
	else if (CheckHitKey(KEY_INPUT_2) && m_activeIndex != 1)
	{
		SwitchActive(1);
	}

	//=======================================================================================
	//移動処理
	float speed = 0.0f;
	bool isMoveInput = false;
	bool isRunInput = false;

	//まずは奥に進むか手前に進むかを判断
	if (CheckHitKey(KEY_INPUT_W))
	{
		speed = -MOVE_SPEED;
		isMoveInput = true;

		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			isRunInput = true;
		}
	}

	if (CheckHitKey(KEY_INPUT_S))
	{
		speed = MOVE_SPEED;
	}

	//入力があったらプレイヤーの現在角度から、移動速度計算
	VECTOR moveSpeed;
	moveSpeed.x = sinf(m_rot.y) * speed;
	moveSpeed.y = 0.0f;
	moveSpeed.z = cosf(m_rot.y) * speed;

	m_pos = VAdd(m_pos, moveSpeed);

	//=======================================================================================
	//移動範囲を制限
	if (m_pos.x < -MOVE_RANGE_X)
	{
		m_pos.x = -MOVE_RANGE_X;
	}
	else if (m_pos.x > MOVE_RANGE_X)
	{
		m_pos.x = MOVE_RANGE_X;
	}

	if (m_pos.z < -MOVE_RANGE_Z)
	{
		m_pos.z = -MOVE_RANGE_Z;
	}
	else if (m_pos.z > MOVE_RANGE_Z)
	{
		m_pos.z = MOVE_RANGE_Z;
	}

	//=======================================================================================
	//回転処理
	if (CheckHitKey(KEY_INPUT_A))
	{
		m_rot.y -= ROT_SPEED;
	}
	else if (CheckHitKey(KEY_INPUT_D))
	{
		m_rot.y += ROT_SPEED;
	}

	//=======================================================================================
	//どちらに切り替えても同じ場所に立つように、両方のキャラへ座標を反映
	for (PlayerCharacter* character : m_characters)
	{
		character->SetPos(m_pos);
		character->SetRot(m_rot);
	}

	//=======================================================================================
	//アクティブなキャラクターのみ攻撃・アニメーションを処理
	bool isAttackInput = CheckHitKey(KEY_INPUT_X);
	PlayerCharacter* active = m_characters[m_activeIndex];

	active->UpdateAnimState(isAttackInput, isMoveInput, isRunInput);

	if (isAttackInput)
	{
		active->Attack(shotManager);
	}
}

//----------------------
//	毎更新
//----------------------
void CharacterManager::Update()
{
	m_characters[m_activeIndex]->Update();
}

//----------------------
//	描画
//----------------------
void CharacterManager::DrawPL()
{
	if (!m_isActive) return;

	m_characters[m_activeIndex]->Draw();
}

//----------------------
//	破棄
//----------------------
void CharacterManager::Fin()
{
	m_char1.Fin();
	m_char2.Fin();
}
