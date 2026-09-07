#pragma once
#include "../ObjectBase/ObjectBase.h"
#include "MeleeCharacter.h"
#include "RangedCharacter.h"

//プレイヤーが操作する複数キャラクターを管理するクラス
class CharacterManager : public ObjectBase {
private:
	MeleeCharacter  m_char1;
	RangedCharacter m_char2;
	//操作対象切り替え用(実体はm_char1/m_char2)
	PlayerCharacter* m_characters[2];
	//現在操作中のキャラクターの添字
	int m_activeIndex;
	//現在の上下速度(ジャンプ・重力で使用)
	float m_velocityY;
	//前フレームのジャンプキー入力状態(押した瞬間だけジャンプさせるための判定用)
	bool m_prevKeySpace;
	//前フレームのスキル/必殺技キー入力状態(押した瞬間だけ発動させるための判定用)
	bool m_prevKeyE;
	bool m_prevKeyR;

	//--- Step() から呼ぶ小さな処理(1つずつ役割を分けている) ---

	//指定した番号の操作キャラクターに切り替える
	void SwitchActive(int index);
	//1/2キーが押されていたら操作キャラクターを切り替える
	void UpdateCharacterSwitch();

	//WASD入力を読んで、カメラの向き基準の移動方向を返す
	//(長さ1にそろえてある。入力が無い/打ち消し合った時は長さ0)
	VECTOR ReadMoveDir(float cameraYaw) const;
	//移動方向と歩き/走りの区別から、このフレームの水平移動量を返す
	VECTOR CalcMoveVelocity(VECTOR moveDir, bool isRun) const;
	//攻撃の踏み込みぶんの移動量を返す(踏み込み中でなければ長さ0)
	VECTOR CalcLungeVelocity(PlayerCharacter* active) const;
	//m_pos を移動可能範囲(フィールド)の中に収める
	void ClampInsideField();
	//移動している方向へ、m_rot.y を少しずつ回して向き直る
	void TurnToward(VECTOR moveDir);
	//ジャンプ入力と重力を処理して m_pos.y / m_velocityY を更新する
	void UpdateVertical(bool isJumpTrigger);

public:
	//コンストラクタ・デストラクタ
	CharacterManager();
	~CharacterManager();

	//初期化
	void Init();
	//データロード
	void Load();
	//毎フレーム計算する処理
	//@cameraYaw : カメラの水平方向の向き(この向き基準でWASD移動する)
	void Step(ShotManager& shotManager, float cameraYaw);
	//毎更新
	void Update();
	//描画
	void DrawPL();
	//破棄
	void Fin();

	//=========================
	//近接攻撃の当たり判定(GameCollisionから使う)
	//=========================
	//攻撃モーション中で、前方に攻撃判定を出すべきかどうか
	bool IsAttackActive() const;
	//攻撃判定(球)の中心座標。キャラの前方に置く
	VECTOR GetAttackPos() const;
	//攻撃判定(球)の半径
	float GetAttackRadius() const;
};
