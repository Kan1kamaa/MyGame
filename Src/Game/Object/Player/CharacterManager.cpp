#include "CharacterManager.h"
#include "math.h"

namespace {
	const float WALK_SPEED = 0.3f;	  //歩きの移動速度
	const float RUN_SPEED = WALK_SPEED * 2.5f;	  //走りの移動速度(歩きの2.5倍)
	const float MOVE_RANGE_X = 300.0f;    //移動範囲を制限
	const float MOVE_RANGE_Z = 300.0f;    //移動範囲を制限
	const float ROT_SPEED = 0.15f;        //キャラが移動方向へ向き直る速さ(1フレームあたりの最大角度)
	const float GRAVITY = 0.3f;           //重力(1フレームごとに上下速度から引く量)
	const float JUMP_POWER = 8.0f;       //ジャンプ初速
}

//----------------------
//	コンストラクタ
//----------------------
CharacterManager::CharacterManager() : m_characters{ &m_char1, &m_char2 }, m_activeIndex(0),
m_velocityY(0.0f), m_prevKeySpace(false), m_prevKeyE(false), m_prevKeyR(false)
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
	m_velocityY = 0.0f;
	m_prevKeySpace = false;
	m_prevKeyE = false;
	m_prevKeyR = false;
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
void CharacterManager::Step(ShotManager& shotManager, float cameraYaw)
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

	//アクティブなキャラクター(移動制限や踏み込みの判定に使うので先に取得しておく)
	PlayerCharacter* active = m_characters[m_activeIndex];
	bool isAttacking = active->IsAttacking();

	//=======================================================================================
	//移動処理(カメラの向き基準。WASDをそのまま前後左右のベクトルとして合成する)
	//攻撃中はWASDでの移動を受け付けない
	VECTOR moveDir = { 0.0f, 0.0f, 0.0f };
	bool isMoveInput = false;
	bool isRunInput = false;

	if (!isAttacking)
	{
		VECTOR forward = { -sinf(cameraYaw), 0.0f, -cosf(cameraYaw) };
		VECTOR right = { -cosf(cameraYaw), 0.0f,  sinf(cameraYaw) };

		if (CheckHitKey(KEY_INPUT_W)) moveDir = VAdd(moveDir, forward);
		if (CheckHitKey(KEY_INPUT_S)) moveDir = VSub(moveDir, forward);
		if (CheckHitKey(KEY_INPUT_D)) moveDir = VAdd(moveDir, right);
		if (CheckHitKey(KEY_INPUT_A)) moveDir = VSub(moveDir, right);

		//入力が打ち消し合って合成ベクトルが0になっていないかで移動判定
		isMoveInput = (VSize(moveDir) > 0.0001f);
		isRunInput = isMoveInput && CheckHitKey(KEY_INPUT_LSHIFT);
	}

	VECTOR moveSpeed = { 0.0f, 0.0f, 0.0f };
	if (isMoveInput)
	{
		//斜め移動が速くなりすぎないよう正規化してから速度を掛ける
		moveDir = VNorm(moveDir);

		//isRunInputがtrueなら(=シフトキーも押されていたら)走りの速さ、そうでなければ歩きの速さを使う
		if (isRunInput)
		{
			moveSpeed = VScale(moveDir, RUN_SPEED);
		}
		else
		{
			moveSpeed = VScale(moveDir, WALK_SPEED);
		}
	}

	//攻撃の踏み込み(自分が向いている方向へ、攻撃モーション序盤だけ少し進む)
	float lungeSpeed = active->GetLungeSpeed();
	if (lungeSpeed != 0.0f)
	{
		VECTOR lungeDir = { -sinf(m_rot.y), 0.0f, -cosf(m_rot.y) };
		moveSpeed = VAdd(moveSpeed, VScale(lungeDir, lungeSpeed));
	}

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
	//回転処理(移動している方向へ、キャラクター自身がなめらかに向き直る)
	if (isMoveInput)
	{
		float targetRot = atan2f(-moveDir.x, -moveDir.z);

		//現在の向きとの差分を-PI〜PIに収め、最短方向で回転させる
		float diff = targetRot - m_rot.y;
		while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
		while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

		if (diff > ROT_SPEED)
		{
			diff = ROT_SPEED;
		}
		else if (diff < -ROT_SPEED)
		{
			diff = -ROT_SPEED;
		}

		m_rot.y += diff;
	}

	//=======================================================================================
	//ジャンプ処理(地面に接地しているときだけジャンプキーを受け付ける。地面はY=0の平面とする)
	bool isKeySpace = CheckHitKey(KEY_INPUT_SPACE) != 0;
	bool isJumpTrigger = isKeySpace && !m_prevKeySpace;
	m_prevKeySpace = isKeySpace;

	bool isGrounded = (m_pos.y <= 0.0f);

	if (isJumpTrigger && isGrounded)
	{
		m_velocityY = JUMP_POWER;
	}

	//重力を適用して上下移動(接地中は毎フレームY=0に戻るだけなので害はない)
	m_velocityY -= GRAVITY;
	m_pos.y += m_velocityY;

	if (m_pos.y <= 0.0f)
	{
		m_pos.y = 0.0f;
		m_velocityY = 0.0f;
	}

	isGrounded = (m_pos.y <= 0.0f); //ジャンプ後の最新状態(アニメーション判定用)

	//=======================================================================================
	//スキル・必殺技(Eキー/Rキーを押した瞬間だけtrueにする)
	bool isKeyE = CheckHitKey(KEY_INPUT_E) != 0;
	bool isSkillTrigger = isKeyE && !m_prevKeyE;
	m_prevKeyE = isKeyE;

	bool isKeyR = CheckHitKey(KEY_INPUT_R) != 0;
	bool isUltTrigger = isKeyR && !m_prevKeyR;
	m_prevKeyR = isKeyR;

	//=======================================================================================
	//どちらに切り替えても同じ場所に立つように、両方のキャラへ座標を反映
	for (int i = 0; i < 2; i++)
	{
		m_characters[i]->SetPos(m_pos);
		m_characters[i]->SetRot(m_rot);
	}

	//=======================================================================================
	//アクティブなキャラクターのみ攻撃・アニメーションを処理
	bool isAttackInput = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	active->UpdateAnimState(isAttackInput, isMoveInput, isRunInput, isGrounded, m_velocityY, isJumpTrigger,
		isSkillTrigger, isUltTrigger);

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
	m_characters[m_activeIndex]->UpdateWeapon();
}

//----------------------
//	描画
//----------------------
void CharacterManager::DrawPL()
{
	if (!m_isActive) return;

	m_characters[m_activeIndex]->Draw();
	m_characters[m_activeIndex]->DrawWeapon();

	//diagnostic: how many animations does this model actually have, and did the last attach succeed?
	PlayerCharacter* active = m_characters[m_activeIndex];
	DrawFormatString(16, 16, GetColor(255, 255, 0), "AnimNum:%d Index:%d AttachID:%d WeaponFrameL:%d WeaponFrameR:%d",
		MV1GetAnimNum(active->m_hndl), active->GetAnimIndex(), active->GetAnimAttachID(),
		active->GetWeaponFrameIndex(), active->GetWeaponFrameIndexR());

	//diagnostic: how many jiggle bones were actually found by name
	DrawFormatString(16, 36, GetColor(255, 255, 0), "JiggleBone found:%d / %d",
		active->GetJiggleFoundCount(), active->GetJiggleTotalCount());
}

//----------------------
//	破棄
//----------------------
void CharacterManager::Fin()
{
	m_char1.Fin();
	m_char2.Fin();
}
