#include"resultScene.h"
#include"DxLib.h"
// コンストラクタ
ResultScene::ResultScene() : m_hndl(-1)
{

}

// 描画全般
void ResultScene::Draw()
{
	DrawGraph(0, 0, m_hndl, TRUE);
}

// 初期化
void ResultScene::Init()
{
	m_state = LOAD;
}
// データロード
void ResultScene::Load()
{
	m_hndl = LoadGraph("Data/Texture/Result/result.png");
	// ロードが終わったらゲーム本編へ
	m_state = START;
}
// メイン処理
void ResultScene::Step()
{
	// エンターキーを押したら終了
	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		m_nextScene = 0;
		m_state = END_WAIT;
	}
}
// データ更新
void ResultScene::Update()
{
}
// 破棄
void ResultScene::Fin()
{
	DeleteGraph(m_hndl);

	// 最初に戻す
	m_state = INIT;
}