#pragma once
#include "DxLib.h"

//Effekseerのエフェクトを読み込み・再生・描画するクラス
//※Effekseer自体の初期化(Effekseer_Init)と終了(Effkseer_End)はMain.cppで行う
class EffectManager {
public:
	//エフェクトの種類
	enum EffectId {
		EFFECT_ATTACK,		//通常攻撃・ジャンプ攻撃の斬撃
		EFFECT_SKILL,		//スキル(氷の柱)
		EFFECT_ULT,			//必殺技

		EFFECT_NUM			// 全エフェクト数
	};

private:
	int m_effectHndl[EFFECT_NUM];	//ロードしたエフェクトのリソースハンドル

public:
	//コンストラクタ・デストラクタ
	EffectManager();
	~EffectManager();

	//初期化
	void Init();
	//エフェクトファイルのロード
	void Load();
	//毎フレーム更新(再生中の全エフェクトを1フレーム進める)
	void Update();
	//描画(カメラの設定が終わった後に呼ぶ)
	void Draw();
	//破棄
	void Fin();

	//エフェクトを再生する
	//@id   : 再生するエフェクト
	//@pos  : 基準となる座標(キャラクターの足元)
	//@rotY : 基準となる向き(キャラクターのY回転。エフェクトはこの方向を向いて出る)
	//@return : 再生中のエフェクトのハンドル(再生失敗は-1)
	int Play(EffectId id, const VECTOR& pos, float rotY);
};
