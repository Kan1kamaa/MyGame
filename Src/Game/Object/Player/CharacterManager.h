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
	//1フレーム前に攻撃キーが押されていたか(トリガー入力判定用)
	bool m_prevAttackKeyDown;

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
	void Step(ShotManager& shotManager);
	//毎更新
	void Update();
	//描画
	void DrawPL();
	//破棄
	void Fin();
};
