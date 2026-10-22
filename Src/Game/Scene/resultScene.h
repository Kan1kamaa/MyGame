#pragma once
#include "SceneBase.h"


// リザルトシーン
class ResultScene : public SceneBase {
protected:
	int m_resultImageHandle;		// 一枚絵のリザルト画像のハンドル

public:
	// コンストラクタ
	ResultScene();

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