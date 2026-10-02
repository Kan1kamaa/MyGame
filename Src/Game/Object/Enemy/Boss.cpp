#include<math.h>
#include"Boss.h"
#include"../Field/Field.h"
#include"../../System/SoundManager.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };

//オリジナルとなるボスのパス
static const char FILE_PATH[] = "Data/models/Enemy/BossGolem.mv1";

static const float BOSS_RAD = 30.0f;		//当たり判定の半径(通常の敵より大きめ)
static const float BOSS_SCALE = 0.8f;		//BossGolemモデルの表示倍率(見た目が大きすぎ/小さすぎる場合はここを調整)
static const float ANIM_SPEED = 0.5f;		//アニメーション再生速度

static const float GRAVITY = 0.4f;           //重力(1フレームごとに上下速度から引く量)
static const float JUMP_POWER = 8.0f;       //ジャンプ初速

static const int   BOSS_MAX_HP = 500;		//最大HP(通常の敵は100)
static const int   BOSS_ATTACK_POWER = 40;	//攻撃力(通常の敵は20)

static const int INVINCIBLE_TIME = 40;        //被弾後の無敵時間(フレーム数。60=約1秒)
static const int HIT_FLASH_TIME = 8;          //被弾してから赤く光らせるフレーム数
//プレイヤーの追跡・攻撃の調整用パラメータ

static const float DETECT_RANGE = 300.0f;		//この距離より近づくとプレイヤーを追いかける
static const float ATTACK_RANGE = 50.0f;		//この距離より近づくと攻撃する
static const float JUMP_RANGE = 150.0f;         //この距離離れてたらジャンプ攻撃
static const float CHASE_MOVE_SPEED = 0.4f;	    //追いかけているときの1フレームあたりの移動量
static const float ROT_SPEED = 0.09f;			//1フレームで向き直れる最大角度(巨体なのでゆっくり)
static const int   ATTACK_COOLDOWN = 100;		//攻撃と攻撃の間隔(フレーム数)
static const int   JUMPATTACK_COOLDOWN = 1200;  //ジャンプ攻撃の間隔
static const float CHARGE_TIME = 0.35;          //アニメーションの初めのタメ時間を判別するのに使用

//ボスの攻撃判定の調整用パラメータ
static const float ATTACK_HIT_START = 0.6f;     //通常攻撃の判定を出し始めるタイミング(アニメ全体に対する割合)
static const float ATTACK_HIT_END = 0.8f;       //通常攻撃の判定を消すタイミング(アニメ全体に対する割合)
static const float ATTACK_HIT_DIST = 20.0f;     //通常攻撃の判定(球)をボスの前方どれだけ先に出すか
static const float ATTACK_HIT_RADIUS = 30.0f;   //通常攻撃の判定(球)の半径
static const int   JUMP_HIT_TIME = 6;           //ジャンプ攻撃の着地の衝撃判定を出すフレーム数
static const float JUMP_HIT_RADIUS = 60.0f;     //ジャンプ攻撃の着地の衝撃判定(球)の半径

//コンストラクタ
BossGolem::BossGolem() :m_speed(VEC_ZERO), m_isDying(false), m_attackCoolCnt(0), m_state(Search)
{
}

//デストラクタ
BossGolem::~BossGolem()
{
	Fin();
}

//初期化
void BossGolem::Init()
{
	ActorBase::Init();
	m_status.Init(BOSS_MAX_HP, BOSS_ATTACK_POWER);
	m_radius = BOSS_RAD;
	m_scale = { BOSS_SCALE, BOSS_SCALE, BOSS_SCALE };
	m_speed = VEC_ZERO;
	m_isDying = false;
	m_attackCoolCnt = 0;
	m_JumpatackCoolCnt = 600;
	m_state = Search;
	m_velocityY = 0;
	m_isJumping = false;
	m_jumpTarget = VEC_ZERO;
	m_jumpMove = VEC_ZERO;
	m_landingHitCnt = 0;
	m_invincibleCnt = 0;
	m_isActive = false;		//Request()で出現させるまで非表示
}

//moveDirの方向へ、m_rot.yを少しずつ回して向き直る(XZ平面)
void BossGolem::TurnToward(const VECTOR& moveDir)
{
	float targetRot = atan2f(-moveDir.x, -moveDir.z);

	//現在の向きとの差分を-PI〜PIに収め、最短方向で回転させる
	float diff = targetRot - m_rot.y;
	while (diff > DX_PI_F)
	{
		diff -= DX_PI_F * 2.0f;
	}
	while (diff < -DX_PI_F)
	{
		diff += DX_PI_F * 2.0f;
	}

	//1フレームで回れる角度に上限をつける
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

//プレイヤーが索敵範囲外にいるときはその場で待機する
void BossGolem::StepSearch()
{
	RequestLoopAnim(ANIM_IDLE, ANIM_SPEED);
	m_speed = VEC_ZERO;
}

//プレイヤーを追いかける
void BossGolem::StepChase(const VECTOR& playerPos)
{
	RequestLoopAnim(ANIM_WALK, ANIM_SPEED);

	VECTOR toPlayer = VSub(playerPos, m_pos);
	toPlayer.y = 0.0f;
	VECTOR dir = VNorm(toPlayer);

	TurnToward(dir);
	m_pos = VAdd(m_pos, VScale(dir, CHASE_MOVE_SPEED));
	m_speed = VScale(dir, CHASE_MOVE_SPEED);
}

//プレイヤーを攻撃する
void BossGolem::StepAttack(const VECTOR& playerPos)
{
	VECTOR toPlayer = playerPos;
	toPlayer.y = 0.0f;
	VECTOR from = m_pos;
	from.y = 0.0f;

	//攻撃中も向きだけはプレイヤーに合わせ続ける
	//TurnToward(VSub(toPlayer, from));
	m_speed = VEC_ZERO;

	//攻撃モーション(通常攻撃1〜3)を再生中ならそのまま最後まで見せる
	bool isAttackAnim = false;
	if (m_animData.m_index == ANIM_ATTACK1)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK2)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK3)
	{
		isAttackAnim = true;
	}
	//最終フレームまで再生し終えたか
	bool isAttackFinished = false;
	if (m_animData.m_nowFrm >= m_animData.m_endFrm)
	{
		isAttackFinished = true;
	}
	if (isAttackAnim == true && isAttackFinished == false)
	{
		return;
	}

	//攻撃と攻撃の間はクールタイムを置いて待機モーションにする
	if (m_attackCoolCnt > 0)
	{
		m_attackCoolCnt--;
		RequestLoopAnim(ANIM_IDLE, ANIM_SPEED);
		return;
	}


	//クールタイムが明けたら、1〜3段目の攻撃モーションをランダムで出す
	TurnToward(VSub(toPlayer, from));
	int pick = ANIM_ATTACK1 + GetRand(2);
	RequestAnim(pick, ANIM_SPEED);
	m_attackCoolCnt = ATTACK_COOLDOWN;
}

//ジャンプ攻撃でプレイヤーを追いかける
void BossGolem::StepJumpAttack(const VECTOR& playerPos)
{
	RequestAnim(ANIM_JUMPATTACK, ANIM_SPEED);

	//空中にいる間は踏み切った瞬間に決めた方向・速さで着地目標へ飛ぶ(プレイヤーを追いかけない)
	if (m_isJumping == true)
	{
		VECTOR toTarget = VSub(m_jumpTarget, m_pos);
		toTarget.y = 0.0f;
		//着地が1フレーム遅れても目標を通り過ぎないように、残り距離が1フレーム分より短ければそこで止める
		if (VSize(toTarget) <= VSize(m_jumpMove))
		{
			m_pos.x = m_jumpTarget.x;
			m_pos.z = m_jumpTarget.z;
			m_speed = VEC_ZERO;
		}
		else
		{
			m_pos = VAdd(m_pos, m_jumpMove);
			m_speed = m_jumpMove;
		}
		return;
	}

	VECTOR toPlayer = VSub(playerPos, m_pos);
	toPlayer.y = 0.0f;
	VECTOR dir = VNorm(toPlayer);

	//タメの間はプレイヤーの方を向き続ける
	TurnToward(dir);
	m_speed = VEC_ZERO;

	if (m_animData.m_nowFrm >= m_animData.m_endFrm * CHARGE_TIME)
	{
		bool isGroundedBeforeGravity = false;
		if (m_pos.y <= Field::GetGroundHeight(m_pos.x, m_pos.z))
		{
			isGroundedBeforeGravity = true;
		}

		if (isGroundedBeforeGravity == true && m_JumpatackCoolCnt <= 0)
		{
			m_velocityY = JUMP_POWER;
			m_JumpatackCoolCnt = JUMPATTACK_COOLDOWN;
			m_isJumping = true;

			//踏み切った瞬間のプレイヤー位置を着地目標として固定する
			m_jumpTarget = playerPos;
			//滞空フレーム数(初速JUMP_POWERから重力で戻ってくるまで)で割って、ちょうど目標に着地する水平速度を求める
			float airFrames = JUMP_POWER * 2.0f / GRAVITY - 1.0f;
			m_jumpMove = VScale(toPlayer, 1.0f / airFrames);

			m_pos = VAdd(m_pos, m_jumpMove);
			m_speed = m_jumpMove;
		}
	}
}

//ロード
void BossGolem::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel(FILE_PATH);
	}
}

//毎フレーム計算する処理
void BossGolem::Step(const VECTOR& playerPos)
{
	//攻撃判定の可視化(デバッグ):攻撃中は前方の当たり判定の球をワイヤーフレームで表示する
	//フラグオフなら終了
	if (m_isActive == false)
	{
		return;
	}

	//死亡モーション再生中は行動せず、再生が終わったら消す
	if (m_isDying == true)
	{
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			m_isActive = false;
			m_isDying = false;
		}
		return;
	}
	//無敵時間とジャンプ攻撃のクールダウン減少
	if (m_invincibleCnt > 0)
	{
		m_invincibleCnt--;
	}
	if (m_JumpatackCoolCnt > 0)
	{
		m_JumpatackCoolCnt--;
	}
	if (m_landingHitCnt > 0)
	{
		m_landingHitCnt--;
	}

	//攻撃モーション(通常攻撃1〜3)を再生中は、距離に関わらず最後まで攻撃状態を維持する
	//(振っている途中でプレイヤーが離れてもChaseに切り替わって歩き出さないようにするため)
	bool isAttackAnim = false;
	if (m_animData.m_index == ANIM_ATTACK1)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK2)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK3)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_JUMPATTACK)
	{
		isAttackAnim = true;
	}
	//最終フレームまで再生し終えたか
	bool isAttackFinished = false;
	if (m_animData.m_nowFrm >= m_animData.m_endFrm)
	{
		isAttackFinished = true;
	}

	//「攻撃状態」で「攻撃アニメを再生中」で「まだ振り終わっていない」の3つが全部揃ったときだけtrue
	bool isMidAttack = false;
	if (m_state == Attack || m_state == JumpAttack)
	{
		if (isAttackAnim == true)
		{
			if (isAttackFinished == false)
			{
				isMidAttack = true;
			}
		}
	}

	if (isMidAttack == false)
	{
		//プレイヤーとの距離で状態を決める
	
		float distToPlayer = VSize(VSub(playerPos, m_pos));
		if (distToPlayer <= ATTACK_RANGE)
		{
			m_state = Attack;
		}
		else if (distToPlayer <= DETECT_RANGE && distToPlayer >= JUMP_RANGE && m_JumpatackCoolCnt  <= 0)
		{
			m_state = JumpAttack;
		}
		else if (distToPlayer <= DETECT_RANGE)
		{
			m_state = Chase;
		}
		/*else if (m_status.NowHp > m_status.MaxHp / 2)
		{
			m_state = Down;
		}*/
		else
		{
			m_state = Search;
		}
	}

	switch (m_state)
	{
	case Search: StepSearch();          break;
	case Chase:  StepChase(playerPos);  break;
	case Attack: StepAttack(playerPos); break;
	case JumpAttack: StepJumpAttack(playerPos); break;
	}
	//重力を適用して上下移動(接地中は毎フレーム地面の高さに戻るだけなので害はない)
	m_velocityY -= GRAVITY;
	m_pos.y += m_velocityY;

	//地面より下には行かないようにする
	float groundHeight = Field::GetGroundHeight(m_pos.x, m_pos.z);
	if (m_pos.y <= groundHeight)
	{
		m_pos.y = groundHeight;
		m_velocityY = 0.0f;
		//ジャンプ攻撃から着地した瞬間に、足元へ衝撃の攻撃判定を数フレーム出す
		if (m_isJumping == true)
		{
			m_landingHitCnt = JUMP_HIT_TIME;
		}
		m_isJumping = false;	//着地したらジャンプ終了
	}
	
}


//描画
void BossGolem::Draw()
{
	if (m_isActive == false)
	{
		return;
	}
	//HP表示
	DrawFormatString(16, 100, GetColor(255, 0, 0), "HP:%d/%d", GetBossHp(), GetBossMaxHp());
	//スタミナ表示
	DrawFormatString(16, 120, GetColor(255, 255, 0), "state:%d", m_state);
	//被弾直後(無敵時間の最初の数フレーム)だけ赤く染めて、攻撃が当たったことをわかりやすくする
	bool isFlash = false;
	if (m_invincibleCnt > INVINCIBLE_TIME - HIT_FLASH_TIME)
	{
		isFlash = true;
	}

	if (isFlash == true)
	{
		MV1SetDifColorScale(m_hndl, GetColorF(1.0f, 0.3f, 0.3f, 1.0f));
	}
	MV1DrawModel(m_hndl);
	if (isFlash == true)
	{
		//他の描画に影響しないよう元の色に戻す
		MV1SetDifColorScale(m_hndl, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//攻撃判定の可視化(デバッグ):判定を出している間は攻撃判定の球をワイヤーフレームで表示する
	if (IsAttackActive() == true)
	{
		DrawSphere3D(GetAttackPos(), GetAttackRadius(), 16,
			GetColor(255, 80, 80), GetColor(0, 0, 0), FALSE);
	}
}

//攻撃判定を出しているか
bool BossGolem::IsAttackActive() const
{
	if (m_isActive == false || m_isDying == true)
	{
		return false;
	}

	//ジャンプ攻撃の着地直後
	if (m_landingHitCnt > 0)
	{
		return true;
	}

	//通常攻撃1〜3は、振りかぶりと振り終わりを除いた振り下ろしの区間だけ判定を出す
	bool isAttackAnim = false;
	if (m_animData.m_index == ANIM_ATTACK1)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK2)
	{
		isAttackAnim = true;
	}
	if (m_animData.m_index == ANIM_ATTACK3)
	{
		isAttackAnim = true;
	}
	if (isAttackAnim == false)
	{
		return false;
	}

	float startFrm = m_animData.m_endFrm * ATTACK_HIT_START;
	float endFrm = m_animData.m_endFrm * ATTACK_HIT_END;
	if (m_animData.m_nowFrm >= startFrm && m_animData.m_nowFrm <= endFrm)
	{
		return true;
	}
	return false;
}

//攻撃判定(球)の中心座標
VECTOR BossGolem::GetAttackPos() const
{
	//ジャンプ攻撃の着地は足元を中心にする
	if (m_landingHitCnt > 0)
	{
		VECTOR center = m_pos;
		center.y += m_radius;
		return center;
	}

	//通常攻撃はボスの前方に置く(TurnTowardと同じ向きの取り方)
	VECTOR forward = { -sinf(m_rot.y), 0.0f, -cosf(m_rot.y) };
	VECTOR center = VAdd(m_pos, VScale(forward, ATTACK_HIT_DIST));
	center.y += m_radius;	//GetCollisionPosと同じく体の高さに合わせる
	return center;
}

//攻撃判定(球)の半径
float BossGolem::GetAttackRadius() const
{
	if (m_landingHitCnt > 0)
	{
		return JUMP_HIT_RADIUS;
	}
	return ATTACK_HIT_RADIUS;
}

//出現させる
bool BossGolem::Request(const VECTOR& pos)
{
	//既に出現中なら終了
	if (m_isActive == true)
	{
		return false;
	}

	m_pos = pos;
	m_status.Init(BOSS_MAX_HP, BOSS_ATTACK_POWER);
	m_state = Search;
	m_isDying = false;
	m_attackCoolCnt = 0;
	m_isJumping = false;
	m_landingHitCnt = 0;
	m_isActive = true;

	return true;
}

void BossGolem::HitCalc(const ObjectBase& other)
{
	//被弾後の無敵時間中はダメージを受けない
	if (m_invincibleCnt > 0)
	{
		return;
	}
	//死亡モーション再生中は追加のダメージ判定をしない
	if (m_isDying == true)
	{
		return;
	}

	m_status.AddDamage(other.GetAttackPower());
	m_invincibleCnt = INVINCIBLE_TIME;
	if (m_status.IsAlive() == false)
	{
		SoundManager::Play(SoundManager::SE_EXPLORE);
		RequestAnim(ANIM_DEATH, ANIM_SPEED);
		m_isDying = true;
		m_speed = VEC_ZERO;		//死亡モーション再生中は動きを止める
	}
}
