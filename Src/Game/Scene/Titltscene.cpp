#include"TitleScene.h"
#include"DxLib.h"
// コンストラクタ
TitleScene::TitleScene() : m_hndl(-1)
{

}

// 描画全般
void TitleScene::Draw()
{
	DrawGraph(0, 0, m_hndl, TRUE);
}

// 初期化
void TitleScene::Init()
{
	m_state = LOAD;
}
// データロード
void TitleScene::Load()
{
	m_hndl = LoadGraph("Data/Texture/Title/title.png");
	// ロードが終わったらゲーム本編へ
	m_state = START;
}
// メイン処理
void TitleScene::Step()
{
	// エンターキーを押したら終了
	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		m_nextScene = 0;
		m_state = END_WAIT;
	}
}
// データ更新
void TitleScene::Update()
{
}
// 破棄
void TitleScene::Fin()
{
	DeleteGraph(m_hndl);

	// 最初に戻す
	m_state = INIT;
}