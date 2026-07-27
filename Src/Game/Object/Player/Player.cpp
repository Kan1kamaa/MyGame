#include "Player.h"
#include "math.h"
#include "../../System/SoundManager.h"
#include "../../Lib/Common.h"
// VECTOR構造体の中身を0にするために使用する
XINPUT_STATE input;
const float ROT_SPEED = 0.05f;	  //回転速度
const float MOVE_SPEED = 1.0f;	  //移動速度
const float MOVE_DASHSPEED = 1.5f;      //ダッシュの速度速度
const float ANIM_SPEED = 1.0f;	  //アニメーションの速度
const float MOVE_RANGE_X = 300.0f;    //移動範囲を制限
const float MOVE_RANGE_Z = 300.0f;    //移動範囲を制限
const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離

// アニメーション一覧
enum tagAnim {


	ANIMID_TPAUSE,
	ANIMID_ATTACK,		//攻撃モーション
	ANIMID_EX,
	ANIMID_HELLO,
	ANIMID_IDLE,	   //待機モーション
	ANIMID_RUN,			//走りモーション
	ANIMID_WALK,		//歩きモーション

	ANIMID_NUM	// 全アニメーション数
};

enum tagAnim2 {


	ANIMID_ATTACK2,		//攻撃モーション
	ANIMID_EX2,
	ANIMID_IDLE2,	   //待機モーション
	ANIMID_RUN2,			//走りモーション
	ANIMID_TPAUSE2,
	ANIMID_WALK2,		//歩きモーション

	ANIMID_NUM2	// 全アニメーション数
};
//----------------------
//	コンストラクタ
//----------------------
Player::Player() : m_speed(VEC_ZERO), m_isMoveSE(false), m_effectPos(),m_state(1),m_hndl2(-1)
{

}

//----------------------
//	デストラクタ
//----------------------
Player::~Player()
{
	Fin();
}

//----------------------
//	初期化処理
//----------------------
void Player::Init()
{

	//ベースクラスにあるInitをやってくれる便利なやつ
	ObjectBase::Init();
	m_radius = 5.0f;
	m_state = 1;
	m_isMoveSE = false;
	m_speed = VEC_ZERO;
	m_actor2.Init();
}

//----------------------
//	データロード
//----------------------
void Player::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Player/charcter/charcter1.mv1");
		//m_hndl2 = MV1LoadModel("Data/Models/Player/charcter2/Kan1kama2.mv1");
		m_actor2.m_hndl = MV1LoadModel("Data/Models/Player/charcter2/Kan1kama2.mv1");;
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
	}
}

void Player::DrawPL()
{
	if (!m_isActive)return;
	if (m_state == 1)
	{
		MV1DrawModel(m_hndl);
	}
	else if (m_state == 2)
	{
		MV1DrawModel(m_actor2.m_hndl);
		//MV1DrawModel(m_hndl2);
	}
}

void Player::Update()
{
	ActorBase::Update();
	MV1SetPosition(m_actor2.m_hndl, m_pos);
	MV1SetRotationXYZ(m_actor2.m_hndl, m_rot);
	MV1SetScale(m_actor2.m_hndl, m_scale);
	m_actor2.Update();
	/*if (m_state == 1)
	{
		MV1SetPosition(m_hndl, m_pos);
		MV1SetRotationXYZ(m_hndl, m_rot);
		MV1SetScale(m_hndl, m_scale);
	}
	else if (m_state == 2)
	{
		MV1SetPosition(m_actor2.m_hndl, m_actor2.m_pos);
		MV1SetRotationXYZ(m_actor2.m_hndl, m_actor2.m_rot);
		MV1SetScale(m_actor2.m_hndl, m_actor2.m_scale);
	}*/

}
//----------------------
//	毎フレーム計算する処理
//----------------------
void Player::Step(ShotManager& shotManager)
{
	if (CheckHitKey(KEY_INPUT_1))
	{
		m_state = 1;
	}
	else if (CheckHitKey(KEY_INPUT_2))
	{
		m_state = 2;
	}
	//アニメーションの切り替え

		switch (m_animData.m_index)
		{
		case ANIMID_IDLE:
			IdleExec();
			break;

		case ANIMID_WALK:
			WalkExec();
			break;

		case ANIMID_RUN:
			RunExec();
			break;

		case ANIMID_ATTACK:
			AttackExec();
			break;

		}
	


	/*if (m_state == 2)
	{
		switch (m_animData.m_index)
		{
		case ANIMID_IDLE2:
			IdleExec2();
			break;

		case ANIMID_WALK2:
			WalkExec2();
			break;

		case ANIMID_RUN2:
			RunExec2();
			break;

		case ANIMID_ATTACK2:
			AttackExec2();
			break;

		}
	}*/
	//=======================================================================================

	//Xを押したら攻撃
	if (CheckHitKey(KEY_INPUT_X))
	{
		VECTOR pos = m_pos;

		//弾の発射場所はプレイヤーの体になるように高さを調整
		pos.y += 10.0f;

		//弾を表示する場所はプレイヤーの回転角度を利用
		pos.x += sinf(m_rot.y) * -ATTACK_DIST;
		pos.z += cosf(m_rot.y) * -ATTACK_DIST;

		//近接攻撃は動かしたくないのでspeedを0にする
		VECTOR speed = { 0.0f,-1.0f,0.0f };
		VECTOR speed2;
		speed2.x = sinf(m_rot.y) * SHOT_SPEED;
		speed2.y = 0.0f;
		speed2.z = cosf(m_rot.y) * SHOT_SPEED;
		//引数のマネージャーから攻撃のリクエスト
		if (m_state == 1)
		{
			shotManager.RequestPlayerAttack(pos, speed);
		}
		else if (m_state == 2)
		{
			shotManager.RequestPlayerShot(pos, speed2);
		}
	}
	//=======================================================================================


	//=======================================================================================
	//移動処理
	float speed = 0.0f;

	//まずは奥に進むか手前に進むかを判断
	if (CheckHitKey(KEY_INPUT_W) || input.Buttons[XINPUT_BUTTON_DPAD_UP])
	{
		speed = -MOVE_SPEED;
		
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			//speed = -MOVE_DASHSPEED;
		}
	}

	if (CheckHitKey(KEY_INPUT_S) || input.Buttons[XINPUT_BUTTON_DPAD_DOWN])
	{
		speed = MOVE_SPEED;

		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{

		}
	}

	//入力した情報とプレイヤーの現在角度から、移動速度計算
	m_speed.x = sinf(m_rot.y) * speed;
	m_speed.y = 0.0f;
	m_speed.z = cosf(m_rot.y) * speed;

	m_pos = VAdd(m_pos, m_speed);
	m_actor2.m_pos = VAdd(m_pos, m_speed);
	//=======================================================================================
	//移動範囲を制限
	if (m_pos.x < -MOVE_RANGE_X)
	{
		m_pos.x = -MOVE_RANGE_X;
		m_actor2.m_pos.x = -MOVE_RANGE_X;
	}
	else if (m_pos.x > MOVE_RANGE_X)
	{
		m_pos.x = MOVE_RANGE_X;
		m_actor2.m_pos.x = MOVE_RANGE_X;
	}

	if (m_pos.z < -MOVE_RANGE_Z)
	{
		m_pos.z = -MOVE_RANGE_Z;
		m_actor2.m_pos.z = -MOVE_RANGE_Z;
	}
	else if (m_pos.z > MOVE_RANGE_Z)
	{
		m_pos.z = MOVE_RANGE_Z;
		m_actor2.m_pos.z = MOVE_RANGE_Z;
	}

	//=======================================================================================


	//=======================================================================================
	//回転処理
	if (CheckHitKey(KEY_INPUT_A) || input.Buttons[XINPUT_BUTTON_DPAD_LEFT])
	{
		m_rot.y -= ROT_SPEED;
		m_actor2.m_rot.y -= ROT_SPEED;
	}
	else if (CheckHitKey(KEY_INPUT_D) || input.Buttons[XINPUT_BUTTON_DPAD_RIGHT])
	{
		m_rot.y += ROT_SPEED;
		m_actor2.m_rot.y += ROT_SPEED;
	}
	//=======================================================================================
}

//----------------------
// 何もしていないとき
//----------------------
void Player::IdleExec()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}
	else if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
			m_actor2.RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
		}
		//歩きモーション
		else
		{
			RequestLoopAnim(ANIMID_WALK, ANIM_SPEED);
			m_actor2.RequestLoopAnim(ANIMID_WALK2, ANIM_SPEED);
		}
	}
	else
	{

	}
}

void Player::IdleExec2()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}
	else if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
			m_actor2.RequestLoopAnim(1, ANIM_SPEED);
		}
		//歩きモーション
		else
		{
			RequestLoopAnim(ANIMID_WALK2, ANIM_SPEED);
		}
	}
	else
	{

	}
}
//----------------------
// 歩き中
//----------------------
void Player::WalkExec()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}
	if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
			m_actor2.RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
		}
		//歩きモーション
		else
		{

		}
	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}

void Player::WalkExec2()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}
	if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
		}
		//歩きモーション
		else
		{

		}
	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}
//----------------------
// 走り中
//----------------------
void Player::RunExec()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}

	if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			//RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
		}
		//歩きモーション
		else
		{
			RequestLoopAnim(ANIMID_WALK, ANIM_SPEED);
			m_actor2.RequestLoopAnim(ANIMID_WALK2, ANIM_SPEED);
		}
	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}

void Player::RunExec2()
{
	//攻撃判定を優先
	if (CheckHitKey(KEY_INPUT_X))
	{
		RequestLoopAnim(ANIMID_ATTACK2, ANIM_SPEED);
	}

	if (CheckHitKey(KEY_INPUT_W))
	{
		//走りモーション
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			//RequestLoopAnim(ANIMID_RUN2, ANIM_SPEED);
		}
		//歩きモーション
		else
		{
			RequestLoopAnim(ANIMID_WALK2, ANIM_SPEED);
		}
	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}
//----------------------
// 攻撃中
//----------------------
void Player::AttackExec()
{
	if (CheckHitKey(KEY_INPUT_X))
	{

	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		m_actor2.RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}

void Player::AttackExec2()
{
	if (CheckHitKey(KEY_INPUT_X))
	{

	}
	else
	{
		RequestLoopAnim(ANIMID_IDLE2, ANIM_SPEED);
	}
}
