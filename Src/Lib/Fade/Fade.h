#pragma once
// フェード一連の流れ
// FADE_NON→FADE_OUT→FADE_OUT_WAITの順で画面が消え、
// FADE_OUT_WAIT→FADE_IN→FADE_NONの順で画面が現れる
class FADE
{
private:
	
	enum tagFade {
		FADE_NON,		// フェードなしの素の状態
		FADE_IN,		// フェードイン中(徐々に明るく)
		FADE_OUT,		// フェードアウト中
		FADE_OUT_WAIT,	// フェードアウト完了後の何も表示されない状態
	};
	static tagFade m_state;	// フェード状態
	static int m_count;		// フェード時間のカウント
public:
	FADE();
	~FADE();
	// フェード初期化
	static void Init();
	// フェード更新
	static void Update();
	// フェード描画
	static void Draw();

	// フェードインリクエスト
	static void RequestFadeIn();
	// フェードアウトリクエスト
	static void RequestFadeOut();
	// フェードインが終了したか？
	static bool IsEndFadeIn();
	// フェードアウトが終了したか？
	static bool IsEndFadeOut();

};

