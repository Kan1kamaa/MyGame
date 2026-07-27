#pragma on
#include "DxLib.h"
#include<vector>

//サウンド操作クラス
class SoundManager {
	
private:
	static std::vector<int>m_hndl;			//サウンドハンドル

public:

	//ゲーム中のBGM.SE一覧
	enum tagSoundID {
		GAME_BGM,		//ゲーム本編のBGM

		SE_PLSHOT,		//プレイヤーの弾発射音
		SE_EXPLORE,		//爆発音

		SOUND_NUM
	};

	//初期化
	static void Init();
	//終了処理
	static void Exit();
	
	//全データ読み込み
	static void Load();

	//音楽再生
	//＠ID：再生する音
	//＠type：再生タイプ　ループさせる場合は[DX_PLAYTYPE_LOOP]を設定
	//＠isStart：true = 最初から再生、false = 続きから再生
	static bool Play(tagSoundID id, int type = DX_PLAYTYPE_BACK, bool isStart = true);
	//音楽停止
	static void Stop(tagSoundID);
	//全音楽停止
	static void StopAll();

	//取得関連
	//サウンドが再生中か？
	static bool IsPlay(tagSoundID id);
	//現在の再生時間取得(ミリ秒)
	static LONGLONG GetSoundTime(tagSoundID id);
	//総再生時間取得(ミリ秒)
	static LONGLONG GetSoundAllTime(tagSoundID id);

	//設定関連
	//再生開始時間設定
	//＠ms：再生開始時間(ミリ秒)
	//＠memo：Play()前に設定し、Play()では途中から再生を指定すること
	static void SetStartFrame(tagSoundID id, int ms);
	//ボリューム設定
	//＠volume：音の大きさ(0.0～1.0で指定)
	//＠memo：音量の最大値は元データの大きさ、元データより大きくはできない
	static void SetVolume(tagSoundID id, float volume);
};