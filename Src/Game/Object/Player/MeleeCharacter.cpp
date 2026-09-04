#include "MeleeCharacter.h"
#include "math.h"
#include "stdio.h"

namespace {
	const float ANIM_SPEED = 0.22f;	  //アニメーションの速度
	const float ATTACK_DIST = 15.0f;     //プレイヤーの前に出す距離
	const float COMBO_CHAIN_RATIO = 0.9f; //この割合を過ぎたら次の段へつなげられる(モーション終盤)
	const float LUNGE_WINDOW_RATIO = 0.35f; //攻撃モーションの最初のこの割合の間だけ踏み込む
	const float LUNGE_SPEED = 0.5f;         //踏み込みの速さ
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

	//揺れものボーンの物理パラメータ(パーツごとに重さ・硬さを変える)
	//{idleAmplitude, idleSpeed, springPower, springDamping, moveReactPower, turnReactPower}
	//項目					上げると						下げると
	//idleAmplitude			待機中の揺れが大きくなる		揺れが小さくなる(0で揺れない)
	//idleSpeed				待機中の揺れが速くせかせかする	ゆっくり大きな周期になる
	//moveReactPower		少し動いただけで大きく傾く		移動に反応しにくくなる
	//turnReactPower		方向転換で大きく振られる		旋回に反応しにくくなる
	//springPower			目標角度に素早く追いつく(硬い)	ゆっくり追いつく(柔らかい)
	//springDamping			揺れが長く続く					すぐ収まる
	//髪・アホ毛:軽くてよく動く
	const JigglePreset JIGGLE_PRESET_HAIR = { 0.02f, 1.0f, 0.15f, 0.75f, 0.05f, 1.0f };
	//背中の服・リボン:髪より少し重い布
	const JigglePreset JIGGLE_PRESET_CLOTH = { 0.02f, 1.5f, 0.08f, 0.85f, 0.04f, 0.8f };
	//スカート:一番重く、ゆっくり大きく揺れる
	const JigglePreset JIGGLE_PRESET_SKIRT = { 0.015f, 1.0f, 0.05f, 0.9f, 0.02f, 0.5f };

	//スカートが脚と貫通しないようにするための、脚のボーン名
	const char* LEG_FRAME_NAME_UPPER_L = "upperleg.L";
	const char* LEG_FRAME_NAME_UPPER_R = "upperleg.R";
	const char* LEG_FRAME_NAME_LOWER_L = "lowerleg.L";
	const char* LEG_FRAME_NAME_LOWER_R = "lowerleg.R";
	const char* LEG_FRAME_NAME_FOOT_L = "foot.L";
	const char* LEG_FRAME_NAME_FOOT_R = "foot.R";
	const char* HIPS_FRAME_NAME = "hips";			//お尻・骨盤のボーン名
	const float LEG_COLLISION_RADIUS = 35.0f;		//脚(太もも・すね)がこの距離より近づいたら押し出す(左右の貫通対策で拡大)
	const float HIPS_COLLISION_RADIUS = 18.0f;		//お尻・骨盤がこの距離より近づいたら押し出す(前後のめくれ対策で縮小)
	const float LEG_PUSH_POWER = 0.2f;				//脚:めり込んだ量を、どれくらいの角度の押し出しに変えるか(左右をさらに強化)
	const float HIPS_PUSH_POWER = 0.04f;			//お尻:同上(前後を少し弱める)
	const float LEG_PUSH_MAX = 0.6f;				//脚の押し出す角度の上限(左右がすぐ頭打ちにならないよう引き上げ)
	const float HIPS_PUSH_MAX = 0.2f;				//お尻の押し出す角度の上限(前後は今まで通り控えめに)

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
m_weaponHndl(-1), m_weaponHndl2(-1), m_handFrameIndexL(-1), m_handFrameIndexR(-1),
m_jiggleBoneNum(0), m_skirtBoneStartIndex(0), m_prevPos{ 0.0f, 0.0f, 0.0f }, m_prevRotY(0.0f), m_jiggleTime(0.0f),
m_legFrameIndexUpperL(-1), m_legFrameIndexUpperR(-1), m_legFrameIndexLowerL(-1), m_legFrameIndexLowerR(-1),
m_legFrameIndexFootL(-1), m_legFrameIndexFootR(-1), m_hipsFrameIndex(-1)
{
}

//初期化
void MeleeCharacter::Init()
{
	ActorBase::Init();
	m_comboStage = 0;
	m_prevAttackInput = false;
	m_comboQueued = false;

	//揺れものボーンの計算に使う、前フレームの座標・向きを今の座標に合わせておく
	//(そうしないと初回だけ大きくジャンプしたような差分になってしまう)
	m_prevPos = m_pos;
	m_prevRotY = m_rot.y;
	m_jiggleTime = 0.0f;
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

		//スカートが貫通しないようにするための、脚のボーン番号を調べておく
		m_legFrameIndexUpperL = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_UPPER_L);
		m_legFrameIndexUpperR = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_UPPER_R);
		m_legFrameIndexLowerL = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_LOWER_L);
		m_legFrameIndexLowerR = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_LOWER_R);
		m_legFrameIndexFootL = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_FOOT_L);
		m_legFrameIndexFootR = MV1SearchFrame(m_hndl, LEG_FRAME_NAME_FOOT_R);
		m_hipsFrameIndex = MV1SearchFrame(m_hndl, HIPS_FRAME_NAME);

		//揺れものボーンをまとめて登録する
		RegisterJiggleBones();
	}

	if (m_weaponHndl == -1)
	{
		m_weaponHndl = MV1LoadModel(WEAPON_FILE_PATH);
		//右手用はデータを共有した複製を作る(ジャンプ攻撃の両手持ちで使う)
		m_weaponHndl2 = MV1DuplicateModel(m_weaponHndl);
	}
}

//終了処理(刀のモデルの後片付けも行う)
void MeleeCharacter::Fin()
{
	ObjectBase::Fin(); //自分のモデル(m_hndl)の後片付け

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

//毎更新(通常の更新に加えて、揺れものボーンの計算もここで行う)
void MeleeCharacter::Update()
{
	ActorBase::Update(); //アニメーションなど、いつも通りの更新

	m_jiggleTime += 1.0f / 60.0f; //経過時間を進める(60フレームで1秒として計算)

	//このフレームで実際に動いた量・回転した量(ワールド座標のまま)
	VECTOR moveDelta = VSub(m_pos, m_prevPos);
	float turnDelta = m_rot.y - m_prevRotY;

	float sinRot = sinf(m_rot.y);
	float cosRot = cosf(m_rot.y);
	VECTOR forward = { -sinRot, 0.0f, -cosRot };
	VECTOR right = { -cosRot, 0.0f,  sinRot };

	VECTOR localMoveDelta;
	localMoveDelta.x = moveDelta.x * right.x + moveDelta.z * right.z;   //右方向にどれだけ動いたか
	localMoveDelta.y = 0.0f;
	localMoveDelta.z = moveDelta.x * forward.x + moveDelta.z * forward.z; //前方向にどれだけ動いたか

	//今再生中のアニメーションに応じた、揺れの強さの倍率
	float animPower = GetJiggleAnimPower();

	//---- スカートが脚・お尻に貫通しないようにする処理(いったん保留) ----
	//まだ左右・前後のバランスが上手く調整できていないため、一旦無効化しておく。
	//再開する時は、このコメントの中身を戻して、下のfor文のbiasRight/biasForwardの
	//決め方を「常に0.0f」から元の計算に戻す。
	/*
	//太もも部分の脚のボーンが揃っているかどうか(1本でも見つかっていなければ当たり判定はしない)
	bool hasUpperLegBones = (m_legFrameIndexUpperL != -1 && m_legFrameIndexUpperR != -1
		&& m_legFrameIndexLowerL != -1 && m_legFrameIndexLowerR != -1);
	//すね部分(膝〜足首)まで判定できるかどうか
	bool hasLowerLegBones = hasUpperLegBones && m_legFrameIndexFootL != -1 && m_legFrameIndexFootR != -1;

	bool hasHipsBone = m_hipsFrameIndex != -1;

	VECTOR legUpperL = { 0.0f, 0.0f, 0.0f };
	VECTOR legLowerL = { 0.0f, 0.0f, 0.0f };
	VECTOR legUpperR = { 0.0f, 0.0f, 0.0f };
	VECTOR legLowerR = { 0.0f, 0.0f, 0.0f };
	VECTOR legFootL = { 0.0f, 0.0f, 0.0f };
	VECTOR legFootR = { 0.0f, 0.0f, 0.0f };
	VECTOR hipsPos = { 0.0f, 0.0f, 0.0f };

	if (hasUpperLegBones)
	{
		//脚のボーンの今の座標(ワールド座標)を取得しておく
		legUpperL = MV1GetFramePosition(m_hndl, m_legFrameIndexUpperL);
		legLowerL = MV1GetFramePosition(m_hndl, m_legFrameIndexLowerL);
		legUpperR = MV1GetFramePosition(m_hndl, m_legFrameIndexUpperR);
		legLowerR = MV1GetFramePosition(m_hndl, m_legFrameIndexLowerR);
	}

	if (hasLowerLegBones)
	{
		legFootL = MV1GetFramePosition(m_hndl, m_legFrameIndexFootL);
		legFootR = MV1GetFramePosition(m_hndl, m_legFrameIndexFootR);
	}

	if (hasHipsBone)
	{
		hipsPos = MV1GetFramePosition(m_hndl, m_hipsFrameIndex);
	}
	*/

	//揺れものボーンを全部更新する
	for (int i = 0; i < m_jiggleBoneNum; i++)
	{
		//脚・お尻への押し出しは保留中なので、常に0(揺れだけそのまま反映する)
		float biasRight = 0.0f;
		float biasForward = 0.0f;

		/*
		//スカートのボーンだけ、脚・お尻に近づきすぎていないかを調べて押し出し量を計算する
		if ((hasUpperLegBones || hasHipsBone) && i >= m_skirtBoneStartIndex && m_jiggleBones[i].IsFound())
		{
			VECTOR bonePos = MV1GetFramePosition(m_hndl, m_jiggleBones[i].GetFrameIndex());
			VECTOR pushWorld = { 0.0f, 0.0f, 0.0f };

			if (hasUpperLegBones)
			{
				//太もも(股関節〜膝)から押し出したい分
				pushWorld = VAdd(pushWorld, ComputeLegPush(bonePos, legUpperL, legLowerL, LEG_COLLISION_RADIUS, LEG_PUSH_POWER, LEG_PUSH_MAX));
				pushWorld = VAdd(pushWorld, ComputeLegPush(bonePos, legUpperR, legLowerR, LEG_COLLISION_RADIUS, LEG_PUSH_POWER, LEG_PUSH_MAX));
			}

			//すね(膝〜足首)からも押し出したい分を追加する(足首のボーンが見つかっている場合のみ)
			if (hasLowerLegBones)
			{
				pushWorld = VAdd(pushWorld, ComputeLegPush(bonePos, legLowerL, legFootL, LEG_COLLISION_RADIUS, LEG_PUSH_POWER, LEG_PUSH_MAX));
				pushWorld = VAdd(pushWorld, ComputeLegPush(bonePos, legLowerR, legFootR, LEG_COLLISION_RADIUS, LEG_PUSH_POWER, LEG_PUSH_MAX));
			}

			//お尻・骨盤からも押し出したい分を追加する(hipsは1点なので、始点と終点を同じにして球として扱う)
			if (hasHipsBone)
			{
				pushWorld = VAdd(pushWorld, ComputeLegPush(bonePos, hipsPos, hipsPos, HIPS_COLLISION_RADIUS, HIPS_PUSH_POWER, HIPS_PUSH_MAX));
			}

			//押し出しベクトルを、キャラの向き基準の「右方向」「前方向」の角度成分に変換する
			biasRight = pushWorld.x * right.x + pushWorld.z * right.z;
			biasForward = pushWorld.x * forward.x + pushWorld.z * forward.z;
		}
		*/

		m_jiggleBones[i].Update(m_hndl, localMoveDelta, turnDelta, m_jiggleTime, animPower, biasRight, biasForward);
	}

	//次のフレームのために、今の座標・向きを覚えておく
	m_prevPos = m_pos;
	m_prevRotY = m_rot.y;
}

//pointから、legTop-legBottomを結ぶ線分までの最も近い点を求める
VECTOR MeleeCharacter::ClosestPointOnSegment(VECTOR point, VECTOR legTop, VECTOR legBottom) const
{
	VECTOR segment = VSub(legBottom, legTop);
	float segmentLengthSq = VDot(segment, segment);

	//太もも・すねの長さがほぼ0(データがおかしい)場合は、太もも側の座標をそのまま返す
	if (segmentLengthSq < 0.0001f)
	{
		return legTop;
	}

	//pointが線分上のどのあたりに一番近いかを0〜1の範囲で求める(0=legTop側、1=legBottom側)
	float t = VDot(VSub(point, legTop), segment) / segmentLengthSq;

	if (t < 0.0f)
	{
		t = 0.0f;
	}
	else if (t > 1.0f)
	{
		t = 1.0f;
	}

	return VAdd(legTop, VScale(segment, t));
}

//bonePosが脚(legTop-legBottom)に近づきすぎている分だけ、外側へ押し出すベクトルを求める
//legTopとlegBottomを同じ座標にすると、1点(球)としての判定になる(hipsで利用)
VECTOR MeleeCharacter::ComputeLegPush(VECTOR bonePos, VECTOR legTop, VECTOR legBottom, float radius, float pushPower, float pushMax) const
{
	VECTOR zero = { 0.0f, 0.0f, 0.0f };

	VECTOR closest = ClosestPointOnSegment(bonePos, legTop, legBottom);
	VECTOR diff = VSub(bonePos, closest);
	float dist = VSize(diff);

	//十分離れている、または脚のちょうど真上にあって方向が定まらない場合は押し出さない
	if (dist >= radius || dist < 0.0001f)
	{
		return zero;
	}

	//めり込んでいる分だけ、外側方向(diffの向き)へ押し出す
	float pushAmount = (radius - dist) * pushPower;

	//深くめり込んでも、めくれ上がったりしないように上限を付ける
	if (pushAmount > pushMax)
	{
		pushAmount = pushMax;
	}

	VECTOR pushDir = VScale(diff, 1.0f / dist); //長さ1に正規化

	return VScale(pushDir, pushAmount);
}

//今再生中のアニメーションに応じた、揺れの強さの倍率を返す(1.0が基準)
float MeleeCharacter::GetJiggleAnimPower() const
{
	switch (m_animData.m_index)
	{
	case ANIMID_IDLE:
		return 1.0f; //待機中は基準の強さ

	case ANIMID_WALK:
		return 1.2f; //歩き中は少し強く

	case ANIMID_RUN:
		return 1.8f; //走り中はさらに強く揺らす

	case ANIMID_ATTACK1:
	case ANIMID_ATTACK2:
	case ANIMID_ATTACK3:
	case ANIMID_SKILL:
	case ANIMID_ULT:
		return 1.5f; //攻撃・スキル・必殺技はメリハリを付けて少し強め

	case ANIMID_JUMSTART:
	case ANIMID_JUMPRISE:
	case ANIMID_JUMPFALL:
	case ANIMID_JUMPATTACK:
	case ANIMID_JUMPRAND:
		return 1.3f; //ジャンプ関連はふわっと強め

	default:
		return 1.0f;
	}
}

//揺れものボーンを1本だけ名前で登録する
void MeleeCharacter::AddJiggleBoneSingle(const char* name, const JigglePreset& preset)
{
	//配列が満杯ならこれ以上登録しない
	if (m_jiggleBoneNum >= JIGGLE_BONE_MAX)
	{
		return;
	}

	//位相(揺れのタイミングをずらす値)は、登録した順番の数字をそのまま使う
	m_jiggleBones[m_jiggleBoneNum].Init(m_hndl, name, (float)m_jiggleBoneNum, preset);
	m_jiggleBoneNum++;
}

//揺れものボーンを連番で登録する(例: prefix.01 〜 prefix.04)
void MeleeCharacter::AddJiggleBoneRange(const char* prefix, int start, int end, const JigglePreset& preset)
{
	for (int i = start; i <= end; i++)
	{
		char name[256];
		sprintf_s(name, sizeof(name), "%s.%02d", prefix, i);
		AddJiggleBoneSingle(name, preset);
	}
}

//揺れものボーンを左右×連番で登録する(例: prefix.L.01〜03、prefix.R.01〜03)
void MeleeCharacter::AddJiggleBoneRangeLR(const char* prefix, int start, int end, const JigglePreset& preset)
{
	for (int i = start; i <= end; i++)
	{
		char nameL[256];
		char nameR[256];
		sprintf_s(nameL, sizeof(nameL), "%s.L.%02d", prefix, i);
		sprintf_s(nameR, sizeof(nameR), "%s.R.%02d", prefix, i);
		AddJiggleBoneSingle(nameL, preset);
		AddJiggleBoneSingle(nameR, preset);
	}
}

//揺れものボーンを「束番号.関節番号」の2段連番で登録する(前髪用。例: prefix.01.01〜08.02)
void MeleeCharacter::AddJiggleBoneRange2(const char* prefix, int outerStart, int outerEnd, int innerStart, int innerEnd, const JigglePreset& preset)
{
	for (int outer = outerStart; outer <= outerEnd; outer++)
	{
		for (int inner = innerStart; inner <= innerEnd; inner++)
		{
			char name[256];
			sprintf_s(name, sizeof(name), "%s.%02d.%02d", prefix, outer, inner);
			AddJiggleBoneSingle(name, preset);
		}
	}
}

//揺れものボーンを全部登録する(合計100本になる想定)
void MeleeCharacter::RegisterJiggleBones()
{
	m_jiggleBoneNum = 0;

	//前髪:jiggle.flonthear.01.01 〜 08.02(8束×2関節=16本)
	AddJiggleBoneRange2("jiggle.flonthear", 1, 8, 1, 2, JIGGLE_PRESET_HAIR);

	//サイドの髪:jiggle.sidehear.L/R.01〜03(左右×3関節=6本)
	AddJiggleBoneRangeLR("jiggle.sidehear", 1, 3, JIGGLE_PRESET_HAIR);
	//サイドの髪2本目:jiggle.sidehear_001.L/R.01〜02(4本)
	AddJiggleBoneRangeLR("jiggle.sidehear_001", 1, 2, JIGGLE_PRESET_HAIR);
	//サイドの髪3本目:jiggle.sidehear_002.L/R.01〜02(4本)
	AddJiggleBoneRangeLR("jiggle.sidehear_002", 1, 2, JIGGLE_PRESET_HAIR);

	//後ろ髪1〜4本目
	AddJiggleBoneRange("jiggle.backhear_001", 1, 3, JIGGLE_PRESET_HAIR); //3本
	AddJiggleBoneRange("jiggle.backhear_002", 1, 2, JIGGLE_PRESET_HAIR); //2本
	AddJiggleBoneRange("jiggle.backhear_003", 1, 2, JIGGLE_PRESET_HAIR); //2本
	AddJiggleBoneRange("jiggle.backhear_004", 1, 2, JIGGLE_PRESET_HAIR); //2本

	//アホ毛
	AddJiggleBoneRange("jiggle.ahoge", 1, 2, JIGGLE_PRESET_HAIR); //2本

	//背中の服/ケープ:jiggle.back_clothes.L/R.01〜04(8本)
	AddJiggleBoneRangeLR("jiggle.back_clothes", 1, 4, JIGGLE_PRESET_CLOTH);
	//ケープ先端の追加分(3桁の005)
	AddJiggleBoneSingle("jiggle.back_clothes.L.005", JIGGLE_PRESET_CLOTH);
	AddJiggleBoneSingle("jiggle.back_clothes.R.005", JIGGLE_PRESET_CLOTH);
	//もう一枚の背中の布:jiggle.back_clothes_001.L/R.01〜03(6本)
	AddJiggleBoneRangeLR("jiggle.back_clothes_001", 1, 3, JIGGLE_PRESET_CLOTH);

	//背中リボン
	AddJiggleBoneSingle("jiggle.backribbon.knot", JIGGLE_PRESET_CLOTH);    //結び目(1本)
	AddJiggleBoneRangeLR("jiggle.backribbon.ear", 1, 1, JIGGLE_PRESET_CLOTH);  //耳(左右で2本)
	AddJiggleBoneRangeLR("jiggle.backribbon.wing", 1, 2, JIGGLE_PRESET_CLOTH); //羽根(4本)
	AddJiggleBoneRangeLR("jiggle.backribbon.tail", 1, 2, JIGGLE_PRESET_CLOTH); //尻尾(4本)

	//スカート(前後左右、各4関節。jiggle.は付かない)
	//ここから先はスカートのボーンなので、脚との当たり判定の対象にする(開始位置を覚えておく)
	m_skirtBoneStartIndex = m_jiggleBoneNum;
	AddJiggleBoneRange("skirt.front", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.back", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.left", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.right", 1, 4, JIGGLE_PRESET_SKIRT);
	//スカート(斜め4方向)
	AddJiggleBoneRange("skirt.front_left", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.front_right", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.back_left", 1, 4, JIGGLE_PRESET_SKIRT);
	AddJiggleBoneRange("skirt.back_right", 1, 4, JIGGLE_PRESET_SKIRT);
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
	velocityY = JUMP_ATTACK_BOOST; //参照渡しなので、CharacterManager側の実際の上下速度も書き換わる
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

//近接攻撃
void MeleeCharacter::Attack(ShotManager& shotManager)
{
	//近接攻撃は一旦弾(判定)を出さないようにしている。あとで当たり判定を作る時にここへ追加する
}

//アニメーション状態の更新
void MeleeCharacter::UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput,
	bool isGrounded, float& velocityY, bool isJumpTrigger,
	bool isSkillTrigger, bool isUltTrigger)
{
	//isAttackInput(今押されているか)がtrueで、m_prevAttackInput(前フレームで押されていたか)がfalseのとき、
	//つまり「今フレームで新しく押された」ときだけisAttackTriggerがtrueになる
	bool isAttackTrigger = isAttackInput && !m_prevAttackInput;
	m_prevAttackInput = isAttackInput; //次のフレームのために、今回の状態を覚えておく

	//地上にいる(待機・歩き・走り)ときだけジャンプ・スキル・必殺技を受け付ける
	//※isGroundedは重力適用後(=ジャンプ開始直後は既に空中扱い)の値なのでここでは使わない
	bool isGroundedState = (m_animData.m_index == ANIMID_IDLE || m_animData.m_index == ANIMID_WALK || m_animData.m_index == ANIMID_RUN);

	//isJumpTriggerがtrueで(=スペースキーを押した瞬間で)、isGroundedStateもtrueなら(=地上にいるなら)ジャンプを開始する
	if (isJumpTrigger && isGroundedState)
	{
		StartJump();
		return; //ジャンプを開始したので、この下のswitch文(待機/歩き/走りなどの処理)は今回はスキップする
	}

	//isSkillTriggerがtrueで(=Eキーを押した瞬間で)、isGroundedStateもtrueなら、スキルを開始する
	if (isSkillTrigger && isGroundedState)
	{
		StartSkill();
		return;
	}

	//isUltTriggerがtrueで(=Rキーを押した瞬間で)、isGroundedStateもtrueなら、必殺技を開始する
	if (isUltTrigger && isGroundedState)
	{
		StartUlt();
		return;
	}

	//m_animData.m_indexは「今再生中のアニメ番号」。これで今の状態(待機/歩き/走り/攻撃/ジャンプ)を判断している
	switch (m_animData.m_index)
	{
	case ANIMID_IDLE: //今は待機モーション中
		//isAttackTriggerがtrueなら(=左クリックを押した瞬間なら)1段目の攻撃を開始する
		if (isAttackTrigger)
		{
			StartAttack(1);
		}
		//攻撃していなくて、isMoveInputがtrueなら(=WASDが押されていたら)歩き/走りへ切り替える
		else if (isMoveInput)
		{
			//isRunInputがtrueなら(=シフトキーも押されていたら)走りモーション、そうでなければ歩きモーション
			if (isRunInput)
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
		if (isAttackTrigger)
		{
			StartAttack(1);
		}
		else if (isMoveInput)
		{
			//歩き中にシフトキーが追加で押されたら走りへ切り替える
			if (isRunInput)
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
		if (isAttackTrigger)
		{
			StartAttack(1);
		}
		else if (isMoveInput)
		{
			//シフトキーが離されたら(isRunInputがfalseになったら)歩きに切り替える
			if (!isRunInput)
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
		//m_nowFrm(今の再生位置)がm_endFrm(このモーションの長さ)以上になった=最後まで再生し終えたら、上昇モーションへ切り替える
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			RequestLoopAnim(ANIMID_JUMPRISE, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPRISE: //今は上昇モーション中
		//isAttackTriggerがtrueなら(=空中で左クリックを押した瞬間なら)空中攻撃を開始する
		if (isAttackTrigger)
		{
			StartJumpAttack(velocityY);
		}
		//isGroundedがfalseで(=まだ地面に着いていなくて)、velocityY(上下速度)が0以下になった=上昇が止まったら、落下モーションへ
		else if (!isGrounded && velocityY <= 0.0f)
		{
			RequestLoopAnim(ANIMID_JUMPFALL, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPFALL: //今は落下モーション中
		if (isAttackTrigger)
		{
			StartJumpAttack(velocityY);
		}
		//isGroundedがtrueになったら(=地面に着いたら)着地モーションへ
		else if (isGrounded)
		{
			RequestAnim(ANIMID_JUMPRAND, ANIM_SPEED);
		}
		break;

	case ANIMID_JUMPATTACK: //今は空中攻撃モーション中
		//着地しても中断せず、必ず最後まで再生する
		if (m_animData.m_nowFrm >= m_animData.m_endFrm)
		{
			//最後まで再生し終えた時点で、地上にいれば待機、まだ空中なら落下モーションへ
			if (isGrounded)
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
		//isAttackTriggerがtrueで(=左クリックを押した瞬間で)、まだ3段目未満なら、
		//すぐには次の段へ切り替えず「次につなげたい」という予約だけしておく
		if (isAttackTrigger && m_comboStage < 3)
		{
			m_comboQueued = true;
		}

		//m_comboQueuedがtrueで(=予約があって)、モーションがCOMBO_CHAIN_RATIO(終盤)まで進んでいたら、次の段を開始する
		if (m_comboQueued && m_animData.m_nowFrm >= m_animData.m_endFrm * COMBO_CHAIN_RATIO)
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

//攻撃モーションの序盤だけ、前方へ踏み込む速度を返す(空中攻撃では踏み込ませない)
float MeleeCharacter::GetLungeSpeed() const
{
	bool isGroundAttack = m_animData.m_index == ANIMID_ATTACK1
		|| m_animData.m_index == ANIMID_ATTACK2
		|| m_animData.m_index == ANIMID_ATTACK3;

	if (!isGroundAttack)
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
	if (!IsAttacking())
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
	if (!IsAttacking())
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

//デバッグ用:揺れものボーンのうち、実際に見つかった本数を数える
int MeleeCharacter::GetJiggleFoundCount() const
{
	int count = 0;

	for (int i = 0; i < m_jiggleBoneNum; i++)
	{
		if (m_jiggleBones[i].IsFound())
		{
			count++;
		}
	}

	return count;
}
