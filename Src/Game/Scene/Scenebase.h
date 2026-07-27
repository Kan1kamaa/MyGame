#pragma once
class SceneBase
{

protected:
	//シーンの一連の流れ
	enum tagState {
		INIT,		//初期化
		LOAD,		//ロード
		START,		//ゲーム開始前に行くシーン
		MAIN,		//ゲーム本編
		END_WAIT,    //本編終了後の処理
		END,		//終了前処理
	};

	tagState m_state;	//現在のシーンの状態
	int m_nextScene;	//次のシーンがどこになるか
public:
	//コンストラクタ
	SceneBase();
	//メインループ
	virtual int Loop();
	//描画
	virtual void Draw() = 0;

private:
	//初期化
	virtual void Init() = 0;
	//ロード
	virtual void Load() = 0;
	//毎フレーム計算する処理
	virtual void Step() = 0;
	//情報更新
	virtual void Update() = 0;
	//破棄
	virtual void Fin() = 0;
};

