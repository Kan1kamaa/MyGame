#include "MeleeCharacter.h"

namespace {
	const float ANIM_SPEED = 0.44f;	  //アニメーションの速度
	const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離
	const float COMBO_CHAIN_RATIO = 0.9f; //この割合を過ぎたら次の段へつなげられる(モーション終盤)
	const float LUNGE_WINDOW_RATIO = 0.35f; //攻撃モーションの最初のこの割合の間だけ踏み込む
	const float LUNGE_SPEED = 1.0f;         //踏み込みの速さ
	const float JUMP_ATTACK_BOOST = 5.0f;   //空中攻撃開始時に与える上向きの速度(最後まで空中で出せるように)

	const char* WEAPON_FILE_PATH = "Data/Models/Weapon/katana2.mv1"; //刀のモデルファイル
	const char* HAND_FRAME_NAME_L = "Hand.L"; //左手のボーン名
	const char* HAND_FRAME_NAME_R = "Hand.R"; //右手のボーン名
	const float WEAPON_SCALE = 1.0f / 8.0f;  //刀のモデルのサイズ調整倍率
	//右手用の補正(通常攻撃・スキル・必殺技で使う刀)
	const float WEAPON_ROT_X = 0.0f;         //刀の傾き(前後)の補正。刃が下を向くよう反転
	const float WEAPON_ROT_Y = DX_PI_F;      //刀の向き(左右)の補正
	const float WEAPON_ROT_Z = 0.0f;         //刃の向き(刀身に沿った回転)の補正

	//左手用の補正(ジャンプ攻撃の両手持ちでだけ使う刀)。
	const float WEAPON_ROT_X_L = 0.0f;
	const float WEAPON_ROT_Y_L = DX_PI_F;
	const float WEAPON_ROT_Z_L = DX_PI_F;    //180度反転

	//アニメーション一覧
	enum tagAnim {
		ANIMID_ATTACK1,		//攻撃1モーション
		ANIMID_ATTACK2,		//攻撃2モーション
		ANIMID_ATTACK3,		//攻撃3モーション
		ANIMID_IDLE,	    //待機モーション
		ANIMID_JUMPATTACK,	//ジャンプ攻撃モーション
		ANIMID_JUMPFALL,    //落下モーション
		ANIMID_JUMPRAND,    //着地モーション
		ANIMID_JUMPRISE,    //上昇モーション
		ANIMID_JUMSTART,    //ジャンプ開始モーション
		ANIMID_RUN,			//走りモーション
		ANIMID_RUNSTART,    //回避モーション
		ANIMID_SKILL,       //スキルモーション
		ANIMID_TPAUSE,      //Tポーズモーション
		ANIMID_ULT,         //必殺技モーション
		ANIMID_WALK,		//歩きモーション
		

		ANIMID_NUM	// 全アニメーション数
	};
}

//----------------------
//	コンストラクタ
//----------------------
MeleeCharacter::MeleeCharacter() : m_comboStage(0), m_prevAttackInput(false), m_comboQueued(false),
m_weaponHndl(-1), m_weaponHndl2(-1), m_handFrameIndexL(-1), m_handFrameIndexR(-1)
{
}

//初期化
void MeleeCharacter::Init()
{
	ActorBase::Init();
	m_comboStage = 0;
	m_prevAttackInput = false;
	m_comboQueued = false;
}

//データロード
void MeleeCharacter::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Player/charcter1/Kan1kama.mv1");
		RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);

		//左手・右手のボーン番号を調べておく(見つからなければ-1のまま)
		m_handFrameIndexL = MV1SearchFrame(m_hndl, HAND_FRAME_NAME_L);
		m_handFrameIndexR = MV1SearchFrame(m_hndl, HAND_FRAME_NAME_R);
	}

	if (m_weaponHndl == -1)
	{
		m_weaponHndl = MV1LoadModel(WEAPON_FILE_PATH);
		//右手用はデータを共有した複製を作る(ジャンプ攻撃の両手持ちで使う)
		m_weaponHndl2 = MV1DuplicateModel(m_weaponHndl);
	}
}

//終了処理
void MeleeCharacter::Fin()
{
	ObjectBase::Fin(); //自分のモデルの後片付け

	if (m_weaponHndl != -1)
	{
		MV1DeleteModel(m_weaponHndl);
		m_weaponHndl = -1;
	}

	if (m_weaponHndl2 != -1)
	{
		MV1DeleteModel(m_weaponHndl2);
		m_weaponHndl2 = -1;
	}
}

//指定した段の攻撃モーションを開始する
void MeleeCharacter::StartAttack(int stage)
{
	m_comboStage = stage;
	m_comboQueued = false;
	//ANIMID_ATTACK1を基準に段数分ずらして再生(ループさせず最終フレームで止める)
	RequestAnim(ANIMID_ATTACK1 + (stage - 1), ANIM_SPEED);
}

//ジャンプ開始モーションを再生する
void MeleeCharacter::StartJump()
{
	RequestAnim(ANIMID_JUMSTART, ANIM_SPEED);
}

//空中攻撃モーションを再生する。velocityYに上向きの速度を入れて、最後まで空中で出し切れるようにする
void MeleeCharacter::StartJumpAttack(float& velocityY)
{
	RequestAnim(ANIMID_JUMPATTACK, ANIM_SPEED);
	velocityY = JUMP_ATTACK_BOOST; 
}

//スキルモーションを再生する
void MeleeCharacter::StartSkill()
{
	RequestAnim(ANIMID_SKILL, ANIM_SPEED);
}

//必殺技モーションを再生する
void MeleeCharacter::StartUlt()
{
	RequestAnim(ANIMID_ULT, ANIM_SPEED);
}

//アニメーション状態の更新
void MeleeCharacter::UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput,
	bool isGrounded, float& velocityY, bool isJumpTrigger,
	bool isSkillTrigger, bool isUltTrigger)
{
	//isAttackInput(今押されているか)がtrueで、m_prevAttackInput(前フレームで押されていたか)がfalseのとき、
	//つまり「今フレームで新しく押された」ときだけisAttackTriggerがtrueになる
	bool isAttackTrigger = (isAttackInput == true && m_prevAttackInput == false);
	m_prevAttackInput = isAttackInput; //次のフレームのために、今回の状態を覚えておく

	//地上にいる(待機・歩き・走り)ときだけジャンプ・スキル・必殺技を受け付ける
	//※isGroundedは重力適用後(=ジャンプ開始直後は既に空中扱い)の値なのでここでは使わない
	bool isGroundedState = (m_animData.m_index == ANIMID_IDLE || m_animData.m_index == ANIMID_WALK || m_animData.m_index == ANIMID_RUN);

	//isJumpTriggerがtrueで(=スペースキーを押した瞬間で)、isGroundedStateもtrueなら(=地上にいるなら)ジャンプを開始する
	if (isJumpTrigger == true && isGroundedState == true)
	{
		StartJump();
		return; //ジャンプを開始したので、この下のswitch文(待機/歩き/走りなどの処理)は今回はスキップする
	}

	//isSkillTriggerがtrueで(=Eキーを押した瞬間で)、isGroundedStateもtrueなら、スキルを開始する
	if (isSkillTrigger == true && isGroundedState == true)
	{
		StartSkill();
		return;
	}

	//isUltTriggerがtrueで(=Rキーを押した瞬間で)、isGroundedStateもtrueなら、必殺技を開始する
	if (isUltTrigger == true && isGroundedState == true)
	{
		StartUlt();
		return;
	}

	//m_animData.m_indexは「今再生中のアニメ番号」。これで今の状態(待機/歩き/走り/攻撃/ジャンプ)を判断している
	switch (m_animData.m_index)
	{
	case ANIMID_IDLE: //今は待機モーション中
		//isAttackTriggerがtrueなら(=左クリックを押した瞬間なら)1段目の攻撃を開始する
		if (isAttackTrigger == true)
		{
			StartAttack(1);
		}
		//攻撃していなくて、isMoveInputがtrueなら(=WASDが押されていたら)歩き/走りへ切り替える
		else if (isMoveInput == true)
		{
			//isRunInputがtrueなら(=シフトキーも押されていたら)走りモーション、そうでなければ歩きモーション
			if (isRunInput == true)
			{
				RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
			}
			else
			{
				RequestLoopAnim(ANIMID_WALK, ANIM_SPEED);
			}
		}
		break;

	case ANIMID_WALK: //今は歩きモーション中
		if (isAttackTrigger == true)
		{
			StartAttack(1);
		}
		else if (isMoveInput == true)
		{
			//歩き中にシフトキーが追加で押されたら走りへ切り替える
			if (isRunInput == true)
			{
				RequestLoopAnim(ANIMID_RUN, ANIM_SPEED);
			}
			//まだWASDは押されているのでそのまま歩き続ける(何もしない)
		}
		//isMoveInputがfalseなら(=もう何も押されていなければ)待機に戻る
		else
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_RUN: //今は走りモーション中
		if (isAttackTrigger == true)
		{
			StartAttack(1);
		}
		else if (isMoveInput == true)
		{
			//シフトキーが離されたら(isRunInputがfalseになったら)歩きに切り替える
			if (isRunInput == false)
			{
				RequestLoopAnim(ANIMID_WALK, ANIM_SPEED);
			}
		}
		else
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMSTART: //今はジャンプ開始モーション中
		//m_nowFrmがm_endFrm以上になったら、上昇モーションへ切り替える
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			RequestLoopAnim(ANIMID_JUMPRISE, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPRISE: //今は上昇モーション中
		//isAttackTriggerがtrueなら空中攻撃を開始する
		if (isAttackTrigger == true)
		{
			StartJumpAttack(velocityY);
		}
		//isGroundedがfalseで、velocityY(上下速度)が0以下になったら、落下モーションへ
		else if (isGrounded == false && velocityY <= 0.0f)
		{
			RequestLoopAnim(ANIMID_JUMPFALL, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPFALL: //今は落下モーション中
		if (isAttackTrigger == true)
		{
			StartJumpAttack(velocityY);
		}
		//isGroundedがtrueになったら着地モーションへ
		else if (isGrounded == true)
		{
			RequestAnim(ANIMID_JUMPRAND, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPATTACK: //今は空中攻撃モーション中
		//着地しても中断せず、必ず最後まで再生する
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			//最後まで再生し終えた時点で、地上にいれば待機、まだ空中なら落下モーションへ
			if (isGrounded == true)
			{
				RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
			}
			else
			{
				RequestLoopAnim(ANIMID_JUMPFALL, ANIM_SPEED);
			}
		}
		break;

	case ANIMID_JUMPRAND: //今は着地モーション中
		//着地モーションを最後まで再生し終えたら待機に戻る
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_SKILL: //今はスキルモーション中
		//スキルモーションを最後まで再生し終えたら待機に戻る
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_ULT: //今は必殺技モーション中
		//必殺技モーションを最後まで再生し終えたら待機に戻る
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;

	case ANIMID_ATTACK1: //1〜3段目のどれかを攻撃中(3つとも同じ処理でまとめて扱う)
	case ANIMID_ATTACK2:
	case ANIMID_ATTACK3:
		//isAttackTriggerがtrueで、まだ3段目未満なら、
		//すぐには次の段へ切り替えず「次につなげたい」という予約だけしておく
		if (isAttackTrigger == true && m_comboStage < 3)
		{
			m_comboQueued = true;
		}

		//m_comboQueuedがtrueで、モーションがCOMBO_CHAIN_RATIOまで進んでいたら、次の段を開始する
		if (m_comboQueued == true && m_animData.m_nowFrm >= m_animData.m_endFrm * COMBO_CHAIN_RATIO)
		{
			StartAttack(m_comboStage + 1);
		}
		//予約が無いまま最後まで再生し終えたら、待機に戻る
		else if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			m_comboStage = 0;
			RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
		}
		break;
	}
}

void MeleeCharacter::ResetToIdle()
{
	m_comboStage = 0;
	m_prevAttackInput = false;
	m_comboQueued = false;
	RequestLoopAnim(ANIMID_IDLE, ANIM_SPEED);
}

//攻撃モーション(地上1〜3段目、空中攻撃)を再生中かどうか。刀を持たせるかどうかの判定にも使う
bool MeleeCharacter::IsAttackMotion() const
{
	return m_animData.m_index == ANIMID_ATTACK1
		|| m_animData.m_index == ANIMID_ATTACK2
		|| m_animData.m_index == ANIMID_ATTACK3
		|| m_animData.m_index == ANIMID_JUMPATTACK;
}

//攻撃モーション・スキル・必殺技のどれかを再生中かどうか
//trueの間はCharacterManager側でWASD移動を受け付けなくなる
bool MeleeCharacter::IsAttacking() const
{
	return IsAttackMotion()
		|| m_animData.m_index == ANIMID_SKILL
		|| m_animData.m_index == ANIMID_ULT;
}

//前方に攻撃の当たり判定を出すべきモーション中かどうか
//通常攻撃1〜3段・ジャンプ攻撃・スキル・必殺技のどれかを再生中ならtrue
bool MeleeCharacter::IsAttackActive() const
{
	return IsAttacking();
}

//攻撃モーションの序盤だけ、前方へ踏み込む速度を返す(空中攻撃では踏み込ませない)
float MeleeCharacter::GetLungeSpeed() const
{
	bool isGroundAttack = m_animData.m_index == ANIMID_ATTACK1
		|| m_animData.m_index == ANIMID_ATTACK2
		|| m_animData.m_index == ANIMID_ATTACK3;

	if (isGroundAttack == false)
	{
		return 0.0f;
	}

	if (m_animData.m_nowFrm < m_animData.m_endFrm * LUNGE_WINDOW_RATIO)
	{
		return LUNGE_SPEED;
	}

	return 0.0f;
}

//刀の座標を手のボーンに合わせて更新する
//通常攻撃・スキル・必殺技は右手だけ、ジャンプ攻撃は両手に持たせる
void MeleeCharacter::UpdateWeapon()
{
	//攻撃・スキル・必殺技のどれでもなければ何もしない
	if (IsAttacking() == false)
	{
		return;
	}

	//刀のモデルサイズを調整する行列と、向き・刃の向きを直す回転行列を先に作っておく
	MATRIX scaleMatrix = MGetScale(VGet(WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE));
	MATRIX rotMatrixX = MGetRotX(WEAPON_ROT_X);
	MATRIX rotMatrixY = MGetRotY(WEAPON_ROT_Y);
	MATRIX rotMatrixZ = MGetRotZ(WEAPON_ROT_Z);
	//サイズ調整 → X回転 → Y回転 → Z回転、の順で先に掛け合わせておく(手の位置・向きへの適用はこのあと)
	MATRIX adjustMatrix = MMult(MMult(MMult(scaleMatrix, rotMatrixX), rotMatrixY), rotMatrixZ);

	//右手のボーンの今の姿勢(ワールド行列)を取得し、サイズ・向きを調整した上で刀のモデルに適用する
	if (m_handFrameIndexR != -1)
	{
		MATRIX handMatrixR = MV1GetFrameLocalWorldMatrix(m_hndl, m_handFrameIndexR);
		MV1SetMatrix(m_weaponHndl, MMult(adjustMatrix, handMatrixR));
	}

	//ジャンプ攻撃の間だけ、左手にも複製した刀を持たせる(左手は右手と鏡合わせなので別の補正を使う)
	if (m_animData.m_index == ANIMID_JUMPATTACK && m_handFrameIndexL != -1)
	{
		MATRIX rotMatrixXL = MGetRotX(WEAPON_ROT_X_L);
		MATRIX rotMatrixYL = MGetRotY(WEAPON_ROT_Y_L);
		MATRIX rotMatrixZL = MGetRotZ(WEAPON_ROT_Z_L);
		MATRIX adjustMatrixL = MMult(MMult(MMult(scaleMatrix, rotMatrixXL), rotMatrixYL), rotMatrixZL);

		MATRIX handMatrixL = MV1GetFrameLocalWorldMatrix(m_hndl, m_handFrameIndexL);
		MV1SetMatrix(m_weaponHndl2, MMult(adjustMatrixL, handMatrixL));
	}
}

//刀を描画する
void MeleeCharacter::DrawWeapon()
{
	if (IsAttacking() == false)
	{
		return;
	}

	if (m_handFrameIndexR != -1)
	{
		MV1DrawModel(m_weaponHndl);
	}

	if (m_animData.m_index == ANIMID_JUMPATTACK && m_handFrameIndexL != -1)
	{
		MV1DrawModel(m_weaponHndl2);
	}
}

