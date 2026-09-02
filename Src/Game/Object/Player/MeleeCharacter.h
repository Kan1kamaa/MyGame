#pragma once
#include "PlayerCharacter.h"
#include "JiggleBone.h"

//揺れものボーンの最大登録数(髪・服・スカートなどで合計100本ほど使うので余裕を持たせる)
const int JIGGLE_BONE_MAX = 110;

//近接攻撃キャラクター(キャラ1)
class MeleeCharacter : public PlayerCharacter {
private:
	int  m_comboStage;      //現在のコンボ段階(0=攻撃していない、1〜3=攻撃中の段)
	bool m_prevAttackInput; //前フレームの攻撃キー入力状態(トリガー判定用)
	bool m_comboQueued;     //次の段への攻撃入力を予約しているか(モーション終盤でつなげる)

	int m_weaponHndl;      //刀のモデルハンドル(左手用)
	int m_weaponHndl2;     //刀のモデルハンドル(右手用。ジャンプ攻撃の両手持ちでのみ使う)
	int m_handFrameIndexL; //左手のボーン(フレーム)番号。見つからなければ-1
	int m_handFrameIndexR; //右手のボーン(フレーム)番号。見つからなければ-1

	JiggleBone m_jiggleBones[JIGGLE_BONE_MAX]; //揺れものボーン一式
	int m_jiggleBoneNum;                       //実際に登録した本数
	int m_skirtBoneStartIndex;                 //m_jiggleBonesの中で、ここから後がスカートのボーン
	VECTOR m_prevPos;                          //前フレームの座標(移動量の計算用)
	float m_prevRotY;                          //前フレームの向き(回転量の計算用)
	float m_jiggleTime;                        //経過時間(待機中の揺れに使う)

	int m_legFrameIndexUpperL; //太もも(左)のボーン番号。見つからなければ-1
	int m_legFrameIndexUpperR; //太もも(右)のボーン番号。見つからなければ-1
	int m_legFrameIndexLowerL; //すね(左)のボーン番号。見つからなければ-1
	int m_legFrameIndexLowerR; //すね(右)のボーン番号。見つからなければ-1
	int m_legFrameIndexFootL;  //足首(左)のボーン番号。見つからなければ-1(すね〜足首の判定に使う)
	int m_legFrameIndexFootR;  //足首(右)のボーン番号。見つからなければ-1
	int m_hipsFrameIndex;      //お尻・骨盤(hips)のボーン番号。見つからなければ-1

	//pointから、legTop-legBottomを結ぶ線分までの最も近い点を求める
	VECTOR ClosestPointOnSegment(VECTOR point, VECTOR legTop, VECTOR legBottom) const;
	//bonePosが脚(legTop-legBottom)に近づきすぎている分だけ、外側へ押し出すベクトルを求める
	//@radius    : これより近づいたら押し出す距離
	//@pushPower : めり込んだ量を、どれくらいの角度の押し出しに変えるか
	//@pushMax   : 押し出す角度の上限(めくれ上がり防止)
	VECTOR ComputeLegPush(VECTOR bonePos, VECTOR legTop, VECTOR legBottom, float radius, float pushPower, float pushMax) const;

	//揺れものボーンを1本だけ名前で登録する
	void AddJiggleBoneSingle(const char* name, const JigglePreset& preset);
	//揺れものボーンを連番で登録する(例: prefix.01 〜 prefix.04)
	void AddJiggleBoneRange(const char* prefix, int start, int end, const JigglePreset& preset);
	//揺れものボーンを左右×連番で登録する(例: prefix.L.01〜03、prefix.R.01〜03)
	void AddJiggleBoneRangeLR(const char* prefix, int start, int end, const JigglePreset& preset);
	//揺れものボーンを「束番号.関節番号」の2段連番で登録する(前髪用。例: prefix.01.01〜08.02)
	void AddJiggleBoneRange2(const char* prefix, int outerStart, int outerEnd, int innerStart, int innerEnd, const JigglePreset& preset);
	//揺れものボーンを全部登録する(Loadから呼ぶ)
	void RegisterJiggleBones();
	//今再生中のアニメーションに応じた、揺れの強さの倍率を返す
	float GetJiggleAnimPower() const;

	//指定した段の攻撃モーションを開始する
	void StartAttack(int stage);
	//ジャンプ開始モーションを再生する
	void StartJump();
	//空中攻撃モーションを再生する。velocityYに上向きの速度を入れて、最後まで空中で出し切れるようにする
	void StartJumpAttack(float& velocityY);
	//スキルモーションを再生する
	void StartSkill();
	//必殺技モーションを再生する
	void StartUlt();
	//攻撃モーション(地上1〜3段目、空中攻撃)を再生中かどうか。刀を持たせるかどうかの判定にも使う
	bool IsAttackMotion() const;

public:
	MeleeCharacter();

	//初期化
	void Init() override;
	//データロード
	void Load();
	//終了処理(刀のモデルの後片付けも行う)
	void Fin();
	//毎更新(揺れものボーンの計算もここで行う)
	void Update() override;

	void Attack(ShotManager& shotManager) override;
	void UpdateAnimState(bool isAttackInput, bool isMoveInput, bool isRunInput,
		bool isGrounded, float& velocityY, bool isJumpTrigger,
		bool isSkillTrigger, bool isUltTrigger) override;
	void ResetToIdle() override;
	bool IsAttacking() const override;
	float GetLungeSpeed() const override;
	void UpdateWeapon() override;
	void DrawWeapon() override;
	int GetWeaponFrameIndex() const override { return m_handFrameIndexL; }
	int GetWeaponFrameIndexR() const override { return m_handFrameIndexR; }
	int GetJiggleFoundCount() const override;
	int GetJiggleTotalCount() const override { return m_jiggleBoneNum; }
};
