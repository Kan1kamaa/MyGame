#include "CharacterManager.h"
#include "math.h"

namespace {
	const float WALK_SPEED = 0.6f;	  //歩きの移動速度
	const float RUN_SPEED = WALK_SPEED * 2.5f;	  //走りの移動速度(歩きの2.5倍)
	const float MOVE_RANGE_X = 300.0f;    //移動範囲を制限
	const float MOVE_RANGE_Z = 300.0f;    //移動範囲を制限
	const float ROT_SPEED = 0.30f;        //キャラが移動方向へ向き直る速さ(1フレームあたりの最大角度)
	const float GRAVITY = 0.6f;           //重力(1フレームごとに上下速度から引く量)
	const float JUMP_POWER = 8.0f;       //ジャンプ初速

	const float ATTACK_HIT_DIST = 14.0f;   //攻撃判定(球)をキャラの前方どれだけ先に出すか
	const float ATTACK_HIT_RADIUS = 14.0f; //攻撃判定(球)の半径

	//キーが「今のフレームで押された瞬間」かどうかを返す。
	//prev には前フレームの押下状態が入っていて、この関数の中で更新する。
	bool IsPressedNow(int keyCode, bool& prev)
	{
		bool now = (CheckHitKey(keyCode) != 0);
		bool triggered = (now == true && prev == false);
		prev = now;
		return triggered;
	}
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
//	毎フレーム計算する処理
//----------------------
//大きく分けて「入力を読む → 移動 → 向き直り → ジャンプ/重力 → 座標反映 → アニメ/攻撃」の順で処理する。
//それぞれの中身は下の小さな関数に分けてある。
void CharacterManager::Step(ShotManager& shotManager, float cameraYaw)
{
	//1/2キーで操作キャラクターを切り替える
	UpdateCharacterSwitch();
	PlayerCharacter* active = m_characters[m_activeIndex];

	//------ このフレームの入力をすべて読む ------
	//攻撃中はWASDでの移動を受け付けない(移動方向を0にする)
	bool isAttacking = active->IsAttacking();
	VECTOR moveDir = (isAttacking == true) ? VGet(0.0f, 0.0f, 0.0f) : ReadMoveDir(cameraYaw);

	bool isMoveInput = (VSize(moveDir) > 0.0001f);
	bool isRunInput = (isMoveInput == true && CheckHitKey(KEY_INPUT_LSHIFT) != 0);
	bool isAttackInput = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	bool isJumpTrigger = IsPressedNow(KEY_INPUT_SPACE, m_prevKeySpace);
	bool isSkillTrigger = IsPressedNow(KEY_INPUT_E, m_prevKeyE);
	bool isUltTrigger = IsPressedNow(KEY_INPUT_R, m_prevKeyR);

	//------ 水平移動(歩き/走り + 攻撃の踏み込み) ------
	VECTOR velocity = CalcMoveVelocity(moveDir, isRunInput);
	velocity = VAdd(velocity, CalcLungeVelocity(active));
	m_pos = VAdd(m_pos, velocity);
	ClampInsideField();

	//------ 移動している方向へ向き直る ------
	if (isMoveInput == true)
	{
		TurnToward(moveDir);
	}

	//------ ジャンプ・重力(縦移動) ------
	UpdateVertical(isJumpTrigger);
	bool isGrounded = (m_pos.y <= 0.0f); //重力適用後の最新の接地状態(アニメーション判定用)

	//------ どちらに切り替えても同じ場所に立つように、両方のキャラへ座標・向きを反映 ------
	for (int i = 0; i < 2; i++)
	{
		m_characters[i]->SetPos(m_pos);
		m_characters[i]->SetRot(m_rot);
	}

	//------ アクティブなキャラクターだけアニメーションと攻撃を処理 ------
	active->UpdateAnimState(isAttackInput, isMoveInput, isRunInput, isGrounded, m_velocityY, isJumpTrigger,
		isSkillTrigger, isUltTrigger);

	if (isAttackInput == true)
	{
		active->Attack(shotManager);
	}
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

//1/2キーが押されていたら操作キャラクターを切り替える
void CharacterManager::UpdateCharacterSwitch()
{
	if (CheckHitKey(KEY_INPUT_1) != 0 && m_activeIndex != 0)
	{
		SwitchActive(0);
	}
	else if (CheckHitKey(KEY_INPUT_2) != 0 && m_activeIndex != 1)
	{
		SwitchActive(1);
	}
}

//WASD入力を読んで、カメラの向き基準の移動方向を返す(長さ1。入力が無ければ長さ0)
VECTOR CharacterManager::ReadMoveDir(float cameraYaw) const
{
	//カメラの向きから「前」と「右」のベクトルを作る
	VECTOR forward = { -sinf(cameraYaw), 0.0f, -cosf(cameraYaw) };
	VECTOR right = { -cosf(cameraYaw), 0.0f,  sinf(cameraYaw) };

	//押されているキーぶんだけ、前後左右のベクトルを足し合わせる
	VECTOR dir = { 0.0f, 0.0f, 0.0f };
	if (CheckHitKey(KEY_INPUT_W) != 0) dir = VAdd(dir, forward);
	if (CheckHitKey(KEY_INPUT_S) != 0) dir = VSub(dir, forward);
	if (CheckHitKey(KEY_INPUT_D) != 0) dir = VAdd(dir, right);
	if (CheckHitKey(KEY_INPUT_A) != 0) dir = VSub(dir, right);

	//入力が無い、または打ち消し合って0になったらそのまま0を返す
	if (VSize(dir) <= 0.0001f)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}

	//斜め移動が速くなりすぎないよう、長さ1にそろえてから返す
	return VNorm(dir);
}

//移動方向と歩き/走りから、このフレームの水平移動量を返す
VECTOR CharacterManager::CalcMoveVelocity(VECTOR moveDir, bool isRun) const
{
	//moveDirが0(動いていない)なら結果も0になる
	float speed = WALK_SPEED;
	if (isRun == true)
	{
		speed = RUN_SPEED;
	}
	return VScale(moveDir, speed);
}

//攻撃の踏み込みぶんの移動量を返す(自分が向いている方向へ、攻撃モーション序盤だけ少し進む)
VECTOR CharacterManager::CalcLungeVelocity(PlayerCharacter* active) const
{
	float lungeSpeed = active->GetLungeSpeed(); //踏み込み中でなければ0が返ってくる
	VECTOR lungeDir = { -sinf(m_rot.y), 0.0f, -cosf(m_rot.y) };
	return VScale(lungeDir, lungeSpeed);
}

//m_pos を移動可能範囲(フィールド)の中に収める
void CharacterManager::ClampInsideField()
{
	if (m_pos.x < -MOVE_RANGE_X) m_pos.x = -MOVE_RANGE_X;
	else if (m_pos.x > MOVE_RANGE_X) m_pos.x = MOVE_RANGE_X;

	if (m_pos.z < -MOVE_RANGE_Z) m_pos.z = -MOVE_RANGE_Z;
	else if (m_pos.z > MOVE_RANGE_Z) m_pos.z = MOVE_RANGE_Z;
}

//移動している方向へ、m_rot.y を少しずつ回して向き直る
void CharacterManager::TurnToward(VECTOR moveDir)
{
	float targetRot = atan2f(-moveDir.x, -moveDir.z);

	//現在の向きとの差分を -PI〜PI に収め、最短方向で回転させる
	float diff = targetRot - m_rot.y;
	while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
	while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

	//1フレームで回れる角度に上限をつける(ROT_SPEED)
	if (diff > ROT_SPEED)       diff = ROT_SPEED;
	else if (diff < -ROT_SPEED) diff = -ROT_SPEED;

	m_rot.y += diff;
}

//ジャンプ入力と重力を処理して m_pos.y / m_velocityY を更新する(地面は Y=0 の平面とする)
void CharacterManager::UpdateVertical(bool isJumpTrigger)
{
	//接地しているときだけジャンプキーを受け付ける
	bool isGroundedBeforeGravity = (m_pos.y <= 0.0f);
	if (isJumpTrigger == true && isGroundedBeforeGravity == true)
	{
		m_velocityY = JUMP_POWER;
	}

	//重力を適用して上下移動(接地中は毎フレームY=0に戻るだけなので害はない)
	m_velocityY -= GRAVITY;
	m_pos.y += m_velocityY;

	//地面より下には行かないようにする
	if (m_pos.y <= 0.0f)
	{
		m_pos.y = 0.0f;
		m_velocityY = 0.0f;
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
	if (m_isActive == false) return;

	m_characters[m_activeIndex]->Draw();
	m_characters[m_activeIndex]->DrawWeapon();

	//デバッグ表示:このモデルのアニメーション数と、直前のアタッチが成功したかを確認する
	PlayerCharacter* active = m_characters[m_activeIndex];
	DrawFormatString(16, 16, GetColor(255, 255, 0), "AnimNum:%d Index:%d AttachID:%d WeaponFrameL:%d WeaponFrameR:%d",
		MV1GetAnimNum(active->m_hndl), active->GetAnimIndex(), active->GetAnimAttachID(),
		active->GetWeaponFrameIndex(), active->GetWeaponFrameIndexR());

	//攻撃判定の可視化(デバッグ):攻撃中は前方の当たり判定の球をワイヤーフレームで表示する
	if (IsAttackActive() == true)
	{
		DrawSphere3D(GetAttackPos(), GetAttackRadius(), 16,
			GetColor(255, 80, 80), GetColor(0, 0, 0), FALSE);
	}
}

//----------------------
//	近接攻撃の当たり判定
//----------------------
//攻撃モーション中で、前方に攻撃判定を出すべきかどうか(実体は操作中のキャラに聞く)
bool CharacterManager::IsAttackActive() const
{
	return m_characters[m_activeIndex]->IsAttackActive();
}

//攻撃判定(球)の中心座標。キャラが向いている方向の少し前に置く
VECTOR CharacterManager::GetAttackPos() const
{
	//キャラの前方ベクトル(移動処理やRangedCharacterと同じ向きの取り方)
	VECTOR forward = { -sinf(m_rot.y), 0.0f, -cosf(m_rot.y) };
	VECTOR center = VAdd(m_pos, VScale(forward, ATTACK_HIT_DIST));
	center.y += m_radius;	//敵のGetCollisionPosと同じく体の高さに合わせる
	return center;
}

//攻撃判定(球)の半径
float CharacterManager::GetAttackRadius() const
{
	return ATTACK_HIT_RADIUS;
}

//----------------------
//	破棄
//----------------------
void CharacterManager::Fin()
{
	m_char1.Fin();
	m_char2.Fin();
}
