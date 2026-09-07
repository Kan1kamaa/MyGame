#include"SoundManager.h"

using namespace std;

//ロードする音楽データのパス
static const char* FILE_PATH[SoundManager::SOUND_NUM] = {
	"Data//Sound//bgm00.mp3","Data//Sound//se_plshot.mp3","Data//Sound//se_explore.mp3"
};

vector<int>SoundManager::m_hndl;  //サウンドハンドル

//初期化
void SoundManager::Init()
{
	//終了処理と同じです
	Exit();
}

//終了処理
void SoundManager::Exit()
{
	for (auto itr = m_hndl.begin(); itr != m_hndl.end(); ++itr)
	{
		if (*itr != -1)
		{
			DeleteSoundMem(*itr);
		}
	}
	//内部データを全て消す
	m_hndl.clear();
}

//全データ読み込み
void SoundManager::Load()
{
	//既にデータが入っているなら終了
	if (m_hndl.size() > 0)return;

	//ループを使って一括ロード
	for (int i = 0; i < SOUND_NUM; i++)
	{
		int hndl = LoadSoundMem(FILE_PATH[i]);
		m_hndl.push_back(hndl);
	}
}

//音楽再生
bool SoundManager::Play(tagSoundID id, int type, bool isStart)
{
	return !PlaySoundMem(m_hndl[id], type, isStart);
}

//音楽停止
void SoundManager::Stop(tagSoundID id)
{
	StopSoundMem(m_hndl[id]);
}

//全音楽停止
void SoundManager::StopAll()
{
	for (auto itr = m_hndl.begin(); itr != m_hndl.end(); ++itr)
	{
		StopSoundMem(*itr);
	}
}

//サウンド再生中か？
bool SoundManager::IsPlay(tagSoundID id)
{
	if (CheckSoundMem(m_hndl[id]) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//現在の再生時取得(ミリ秒)
LONGLONG SoundManager::GetSoundTime(tagSoundID id)
{
	return GetSoundCurrentTime(m_hndl[id]);
}

//総再生時間取得(ミリ秒)
LONGLONG SoundManager::GetSoundAllTime(tagSoundID id)
{
	return GetSoundTotalTime(m_hndl[id]);
}

//再生開始時間設定
void SoundManager::SetStartFrame(tagSoundID id, int ms)
{
	//再生開始時間をミリ秒からSetCurrentPositionSoundMemの引数の形に変換
	int freq = GetFrequencySoundMem(m_hndl[id]) * ms / 1000;
	SetCurrentPositionSoundMem(freq, m_hndl[id]);
}

//再生開始時間設定
void SoundManager::SetVolume(tagSoundID id, float volume)
{
	//DxLibの引数は0〜255の間なので、少し計算を入れている
	ChangeVolumeSoundMem((int)(255.0f * volume), m_hndl[id]);
}
