#pragma once
#include "SceneBase.h"


// ゲーム本編管理シーン
class TitleScene : public SceneBase {
protected:
	int m_hndl;		// 一枚絵のタイトル画像を使う

public:
	// コンストラクタ
	TitleScene();

	// 描画全般
	void Draw();

protected:
	// 初期化
	void Init();
	// データロード
	void Load();
	// メイン処理
	void Step();
	// データ更新
	void Update();
	// 破棄
	void Fin();
};