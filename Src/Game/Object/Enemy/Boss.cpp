#include<math.h>
#include"Boss.h"
#include"../../System/SoundManager.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };

//オリジナルとなるボスのパス
static const char FILE_PATH[] = "Data/models/Enemy/BossGolem.mv1";

static const float BOSS_RAD = 30.0f;		//当たり判定の半径(通常の敵より大きめ)
static const float BOSS_SCALE = 0.6f;		//BossGolemモデルの表示倍率(見た目が大きすぎ/小さすぎる場合はここを調整)
static const float ANIM_SPEED = 0.5f;		//アニメーション再生速度

static const int   BOSS_MAX_HP = 1000;		//最大HP(通常の敵は100)
static const int   BOSS_ATTACK_POWER = 40;	//攻撃力(通常の敵は20)

//プレイヤーの追跡・攻撃の調整用パラメータ
static const float DETECT_RANGE = 200.0f;		//この距離より近づくとプレイヤーを追いかける
static const float ATTACK_RANGE = 50.0f;		//この距離より近づくと攻撃する
static const float CHASE_MOVE_SPEED = 0.4f;	//追いかけているときの1フレームあたりの移動量
static const float ROT_SPEED = 0.06f;			//1フレームで向き直れる最大角度(巨体なのでゆっくり)
static const int   ATTACK_COOLDOWN = 40;		//攻撃と攻撃の間隔(フレーム数)

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
	m_state = Search;
	m_isActive = false;		//Request()で出現させるまで非表示
}

//moveDirの方向へ、m_rot.yを少しずつ回して向き直る(XZ平面)
void BossGolem::TurnToward(const VECTOR& moveDir)
{
	float targetRot = atan2f(-moveDir.x, -moveDir.z);

	//現在の向きとの差分を-PI〜PIに収め、最短方向で回転させる
	float diff = targetRot - m_rot.y;
	while (diff > DX_PI_F)  diff -= DX_PI_F * 2.0f;
	while (diff < -DX_PI_F) diff += DX_PI_F * 2.0f;

	//1フレームで回れる角度に上限をつける
	if (diff > ROT_SPEED)       diff = ROT_SPEED;
	else if (diff < -ROT_SPEED) diff = -ROT_SPEED;

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
	TurnToward(VSub(toPlayer, from));
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
	bool isAttackFinished = (m_animData.m_nowFrm >= m_animData.m_endFrm);
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
	int pick = ANIM_ATTACK1 + GetRand(2);
	RequestAnim(pick, ANIM_SPEED);
	m_attackCoolCnt = ATTACK_COOLDOWN;
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
	//フラグオフなら終了
	if (m_isActive == false)return;

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
	bool isAttackFinished = (m_animData.m_nowFrm >= m_animData.m_endFrm);

	//「攻撃状態」で「攻撃アニメを再生中」で「まだ振り終わっていない」の3つが全部揃ったときだけtrue
	bool isMidAttack = false;
	if (m_state == Attack)
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
		else if (distToPlayer <= DETECT_RANGE)
		{
			m_state = Chase;
		}
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
	}
}

//出現させる
bool BossGolem::Request(const VECTOR& pos)
{
	//既に出現中なら終了
	if (m_isActive == true)return false;

	m_pos = pos;
	m_status.Init(BOSS_MAX_HP, BOSS_ATTACK_POWER);
	m_state = Search;
	m_isDying = false;
	m_attackCoolCnt = 0;
	m_isActive = true;

	return true;
}

void BossGolem::HitCalc(const ObjectBase& other)
{
	//死亡モーション再生中は追加のダメージ判定をしない
	if (m_isDying == true)return;

	m_status.AddDamage(other.GetAttackPower());
	if (m_status.IsAlive() == false)
	{
		SoundManager::Play(SoundManager::SE_EXPLORE);
		RequestAnim(ANIM_DEATH, ANIM_SPEED);
		m_isDying = true;
		m_speed = VEC_ZERO;		//死亡モーション再生中は動きを止める
	}
}
