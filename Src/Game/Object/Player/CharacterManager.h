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

	//操作キャラクターの切り替え
	void SwitchActive(int index);

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
};
