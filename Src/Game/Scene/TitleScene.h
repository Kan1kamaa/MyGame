#pragma once
#include "SceneBase.h"


// タイトルシーン
class TitleScene : public SceneBase {
protected:
	int m_titleImageHandle;		// 一枚絵のタイトル画像のハンドル

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