#include "CharacterManager.h"
#include "../Field/Field.h"
#include "math.h"

namespace {
	const int PLAYER_MAX_HP = 100;					//プレイヤーの最大HP
	const float PLAYER_MAX_STAMINA = 100;			//プレイヤーの最大スタミナ
	const float JUMP_POWER = 8.0f;					//ジャンプ初速
	const float WALK_SPEED = 0.6f;					//歩きの移動速度
	const float RUN_SPEED = WALK_SPEED * 2.5f;		//走りの移動速度(歩きの2.5倍)
	const float MOVE_RANGE_X = 300.0f;				//移動範囲を制限
	const float MOVE_RANGE_Z = 300.0f;				//移動範囲を制限
	const float ROT_SPEED = 0.30f;					//キャラが移動方向へ向き直る速さ(1フレームあたりの最大角度)
	const float GRAVITY = 0.6f;					    //重力(1フレームごとに上下速度から引く量)
	const float DASH_STAMINA_COST = 0.2f;			//ダッシュ中に1フレームで減る量(100なら約3.3秒走れる)
	const float AVOID_STAMINA_COST = 10.0f;			//回避1回で減る量
	const float JUMP_STAMINA_COST = 10.0f;			//ジャンプ1回で減る量
	const float STAMINA_REGEN = 0.3f;				//1フレームで回復する量
	const int STAMINA_REGENDELAY = 120;				//スタミナが回復を始めるまでの時間
	const float DASH_RESTART_STAMINA = 30;			//息切れから走れるようになる量
	const int INVINCIBLE_TIME = 60;					//被弾後の無敵時間(フレーム数。60=約1秒)

	const float ATTACK_HIT_DIST = 14.0f;			//攻撃判定(球)をキャラの前方どれだけ先に出すか
	const float ATTACK_HIT_RADIUS = 14.0f;			//攻撃判定(球)の半径

	
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
}

//----------------------
//	コンストラクタ
//----------------------
CharacterManager::CharacterManager() :
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
	m_velocityY = 0.0f;
	m_prevKeySpace = false;
	m_prevKeyE = false;
	m_prevKeyR = false;
	m_status.Init(PLAYER_MAX_HP, 0);
	m_stamina = PLAYER_MAX_STAMINA;
	m_staminaRegenWait = 0;
	m_invincibleCnt = 0;
	m_isExhausted = false;
	m_wasDodging = false;
	m_char.Init();
}

//----------------------
//	データロード
//----------------------
void CharacterManager::Load()
{
	m_char.Load();
}

//----------------------
//	毎フレーム計算する処理
//----------------------
//大きく分けて「入力を読む → 移動 → 向き直り → ジャンプ/重力 → 座標反映 → アニメ/攻撃」の順で処理する。
//それぞれの中身は下の小さな関数に分けてある。
void CharacterManager::Step(float cameraYaw)
{
	//無敵時間を減らす
	if (m_invincibleCnt > 0)
	{
		m_invincibleCnt--;
	}
	if (m_staminaRegenWait > 0)
	{
		m_staminaRegenWait--;
	}
	else
	{
		if (m_stamina < PLAYER_MAX_STAMINA)
		{
			m_stamina += STAMINA_REGEN;
		}
	}
	
	//------ このフレームの入力をすべて読む ------
	//攻撃中はWASDでの移動を受け付けない(移動方向を0にする)
	bool isAttacking = m_char.IsAttacking();
	VECTOR moveDir = VGet(0.0f, 0.0f, 0.0f);
	if (isAttacking == false)
	{
		//攻撃中でなければWASDから移動方向を作る
		moveDir = ReadMoveDir(cameraYaw);
	}

	//移動方向の長さが0より大きければ(=WASDが押されていれば)移動入力あり
	bool isMoveInput = false;
	if (VSize(moveDir) > 0.0001f)
	{
		isMoveInput = true;
	}
	//スタミナが0になったら息切れ。30まで回復したら解除
	if (m_stamina < 0)
	{
		m_isExhausted = true;
	}
	if (m_stamina >= DASH_RESTART_STAMINA)
	{
		m_isExhausted = false;
	}

	//移動中に左シフトも押されていれば走り(息切れ中は走れない)
	bool isRunInput = false;
	if (isMoveInput == true)
	{
		if (CheckHitKey(KEY_INPUT_LSHIFT) != 0 &&m_isExhausted == false)
		{
			isRunInput = true;
			m_stamina -= DASH_STAMINA_COST;
			m_staminaRegenWait = STAMINA_REGENDELAY;
		}
	}
	if (m_stamina < AVOID_STAMINA_COST)
	{
		isRunInput = false;
	}
	//マウスの左ボタンが押されていれば攻撃入力あり(押している間ずっとtrue)
	bool isAttackInput = false;
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		isAttackInput = true;
	}

	//ジャンプ・スキル・必殺技はキーを押した瞬間だけtrue
	bool isJumpTrigger = IsPressedNow(KEY_INPUT_SPACE, m_prevKeySpace);
	if (m_stamina < JUMP_STAMINA_COST)
	{
		isJumpTrigger = false;
	}
	bool isSkillTrigger = IsPressedNow(KEY_INPUT_E, m_prevKeyE);
	bool isUltTrigger = IsPressedNow(KEY_INPUT_R, m_prevKeyR);

	//------ 水平移動(歩き/走り + 攻撃の踏み込み) ------
	VECTOR velocity = CalcMoveVelocity(moveDir, isRunInput);
	velocity = VAdd(velocity, CalcLungeVelocity());
	m_pos = VAdd(m_pos, velocity);
	ClampInsideField();

	//------ 移動している方向へ向き直る ------
	if (isMoveInput == true)
	{
		TurnToward(moveDir);
	}

	//------ ジャンプ・重力(縦移動) ------
	UpdateVertical(isJumpTrigger);
	//重力適用後の最新の接地状態(アニメーション判定用)
	bool isGrounded = false;
	if (m_pos.y <= Field::GetGroundHeight(m_pos.x, m_pos.z))
	{
		isGrounded = true;
	}

	//------ キャラクターへ座標・向きを反映 ------
	m_char.SetPos(m_pos);
	m_char.SetRot(m_rot);

	//------ アニメーションを処理 ------
	m_char.UpdateAnimState(isAttackInput, isMoveInput, isRunInput, isGrounded, m_velocityY, isJumpTrigger,
		isSkillTrigger, isUltTrigger);

	//------ 回避(走り出し)に切り替わった瞬間だけ、まとめてスタミナを引く ------
	bool isDodging = m_char.IsDodging();
	if (isDodging == true && m_wasDodging == false)
	{
		m_stamina -= AVOID_STAMINA_COST;
		m_staminaRegenWait = STAMINA_REGENDELAY;
	}
	m_wasDodging = isDodging;
}

//WASD入力を読んで、カメラの向き基準の移動方向を返す(長さ1。入力が無ければ長さ0)
VECTOR CharacterManager::ReadMoveDir(float cameraYaw) const
{
	//カメラの向きから「前」と「右」のベクトルを作る
	VECTOR forward = { -sinf(cameraYaw), 0.0f, -cosf(cameraYaw) };
	VECTOR right = { -cosf(cameraYaw), 0.0f,  sinf(cameraYaw) };

	//押されているキーぶんだけ、前後左右のベクトルを足し合わせる
	VECTOR dir = { 0.0f, 0.0f, 0.0f };
	//Wなら前へ
	if (CheckHitKey(KEY_INPUT_W) != 0)
	{
		dir = VAdd(dir, forward);
	}
	//Sなら後ろへ
	if (CheckHitKey(KEY_INPUT_S) != 0)
	{
		dir = VSub(dir, forward);
	}
	//Dなら右へ
	if (CheckHitKey(KEY_INPUT_D) != 0)
	{
		dir = VAdd(dir, right);
	}
	//Aなら左へ
	if (CheckHitKey(KEY_INPUT_A) != 0)
	{
		dir = VSub(dir, right);
	}

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
VECTOR CharacterManager::CalcLungeVelocity() const
{
	float lungeSpeed = m_char.GetLungeSpeed(); //踏み込み中でなければ0が返ってくる
	VECTOR lungeDir = { -sinf(m_rot.y), 0.0f, -cosf(m_rot.y) };
	return VScale(lungeDir, lungeSpeed);
}

//m_pos を移動可能範囲(フィールド)の中に収める
void CharacterManager::ClampInsideField()
{
	//X方向:左端より外なら左端に、右端より外なら右端に戻す
	if (m_pos.x < -MOVE_RANGE_X)
	{
		m_pos.x = -MOVE_RANGE_X;
	}
	else if (m_pos.x > MOVE_RANGE_X)
	{
		m_pos.x = MOVE_RANGE_X;
	}

	//Z方向:手前端より外なら手前端に、奥端より外なら奥端に戻す
	if (m_pos.z < -MOVE_RANGE_Z)
	{
		m_pos.z = -MOVE_RANGE_Z;
	}
	else if (m_pos.z > MOVE_RANGE_Z)
	{
		m_pos.z = MOVE_RANGE_Z;
	}
}

//移動している方向へ、m_rot.y を少しずつ回して向き直る
void CharacterManager::TurnToward(VECTOR moveDir)
{
	float targetRot = atan2f(-moveDir.x, -moveDir.z);

	//現在の向きとの差分を -PI〜PI に収め、最短方向で回転させる
	float diff = targetRot - m_rot.y;
	while (diff > DX_PI_F)
	{
		diff -= DX_PI_F * 2.0f;
	}
	while (diff < -DX_PI_F)
	{
		diff += DX_PI_F * 2.0f;
	}

	//1フレームで回れる角度に上限をつける(ROT_SPEED)
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

//ジャンプ入力と重力を処理して m_pos.y / m_velocityY を更新する
void CharacterManager::UpdateVertical(bool isJumpTrigger)
{
	float groundHeight = Field::GetGroundHeight(m_pos.x, m_pos.z);

	//接地しているときだけジャンプキーを受け付ける
	bool isGroundedBeforeGravity = false;
	if (m_pos.y <= groundHeight)
	{
		isGroundedBeforeGravity = true;
	}
	if (isJumpTrigger == true && isGroundedBeforeGravity == true)
	{
		m_velocityY = JUMP_POWER;
		m_stamina -= JUMP_STAMINA_COST;
		m_staminaRegenWait = STAMINA_REGENDELAY;
	}

	//重力を適用して上下移動(接地中は毎フレーム地面の高さに戻るだけなので害はない)
	m_velocityY -= GRAVITY;
	m_pos.y += m_velocityY;

	//地面より下には行かないようにする
	if (m_pos.y <= groundHeight)
	{
		m_pos.y = groundHeight;
		m_velocityY = 0.0f;
	}
}

//----------------------
//	毎更新
//----------------------
void CharacterManager::Update()
{
	m_char.Update();
	m_char.UpdateWeapon();
}

//----------------------
//	描画
//----------------------
void CharacterManager::DrawPL()
{
	//行動不能(HP0)なら描画しない
	if (m_isActive == false)
	{
		return;
	}

	m_char.Draw();
	m_char.DrawWeapon();

	//デバッグ表示:このモデルのアニメーション数と、直前のアタッチが成功したかを確認する
	DrawFormatString(16, 16, GetColor(255, 255, 0), "AnimNum:%d Index:%d AttachID:%d WeaponFrameL:%d WeaponFrameR:%d",
		MV1GetAnimNum(m_char.m_hndl), m_char.GetAnimIndex(), m_char.GetAnimAttachID(),
		m_char.GetWeaponFrameIndex(), m_char.GetWeaponFrameIndexR());

	//HP表示
	DrawFormatString(16, 650, GetColor(255, 0, 0), "HP:%d/%d", GetHp(), GetMaxHp());
	//スタミナ表示
	DrawFormatString(16, 670, GetColor(255, 255, 0), "スタミナ:%d/100", (int)m_stamina);
	//攻撃判定の可視化(デバッグ):攻撃中は前方の当たり判定の球をワイヤーフレームで表示する
	if (IsAttackActive() == true)
	{
		DrawSphere3D(GetAttackPos(), GetAttackRadius(), 16,
			GetColor(255, 80, 80), GetColor(0, 0, 0), FALSE);
	}
}

//----------------------
//	被弾処理
//----------------------
//無敵時間中・回避モーション中でなければダメージを受ける。HPが0になったら行動不能にする
void CharacterManager::HitCalc(const ObjectBase& other)
{
	//被弾後の無敵時間中はダメージを受けない
	if (m_invincibleCnt > 0)
	{
		return;
	}
	//走り出し(回避)モーション中は無敵
	if (m_char.IsDodging() == true)
	{
		return;
	}

	m_status.AddDamage(other.GetAttackPower());
	m_invincibleCnt = INVINCIBLE_TIME;

	if (m_status.IsAlive() == false)
	{
		m_isActive = false;
	}
}

//現在操作中のキャラクターの攻撃力を返す(近接攻撃が敵に当たったときに使われる)
float CharacterManager::GetAttackPower() const
{
	return m_char.GetAttackPower();
}

//----------------------
//	近接攻撃の当たり判定
//----------------------
//攻撃モーション中で、前方に攻撃判定を出すべきかどうか(実体は操作中のキャラに聞く)
bool CharacterManager::IsAttackActive() const
{
	return m_char.IsAttackActive();
}

//攻撃判定(球)の中心座標。キャラが向いている方向の少し前に置く
VECTOR CharacterManager::GetAttackPos() const
{
	//キャラの前方ベクトル(移動処理と同じ向きの取り方)
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
	m_char.Fin();
}
