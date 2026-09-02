#include"SoundManager.h"

using namespace std;

//���[�h���鉹�y�f�[�^�̃p�X
static const char* FILE_PATH[SoundManager::SOUND_NUM] = {
	"Data//Sound//bgm00.mp3","Data//Sound//se_plshot.mp3","Data//Sound//se_explore.mp3"
};

vector<int>SoundManager::m_hndl;  //�T�E���h�n���h��

//������
void SoundManager::Init()
{
	//�I�������Ɠ����ł�
	Exit();
}

//�I������
void SoundManager::Exit()
{
	for (auto itr = m_hndl.begin(); itr != m_hndl.end(); ++itr)
	{
		if (*itr != -1)
		{
			DeleteSoundMem(*itr);
		}
	}
	//�����f�[�^��S�ď���
	m_hndl.clear();
}

//�S�f�[�^�ǂݍ���
void SoundManager::Load()
{
	//���Ƀf�[�^�������Ă���Ȃ�I��
	if (m_hndl.size() > 0)return;

	//���[�v���g���Ĉꊇ���[�h
	for (int i = 0; i < SOUND_NUM; i++)
	{
		int hndl = LoadSoundMem(FILE_PATH[i]);
		m_hndl.push_back(hndl);
	}
}

//���y�Đ�
bool SoundManager::Play(tagSoundID id, int type, bool isStart)
{
	return !PlaySoundMem(m_hndl[id], type, isStart);
}

//���y��~
void SoundManager::Stop(tagSoundID id)
{
	StopSoundMem(m_hndl[id]);
}

//�S���y��~
void SoundManager::StopAll()
{
	for (auto itr = m_hndl.begin(); itr != m_hndl.end(); ++itr)
	{
		StopSoundMem(*itr);
	}
}

//�T�E���h�Đ������H
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

//���݂̍Đ����擾(�~���b)
LONGLONG SoundManager::GetSoundTime(tagSoundID id)
{
	return GetSoundCurrentTime(m_hndl[id]);
}

//���Đ����Ԏ擾(�~���b)
LONGLONG SoundManager::GetSoundAllTime(tagSoundID id)
{
	return GetSoundTotalTime(m_hndl[id]);
}

//�Đ��J�n���Ԑݒ�
void SoundManager::SetStartFrame(tagSoundID id, int ms)
{
	//�Đ��J�n���Ԃ��~���b����SetCurrentPositionSoundMem�̈����̌`�ɕϊ�
	int freq = GetFrequencySoundMem(m_hndl[id]) * ms / 1000;
	SetCurrentPositionSoundMem(freq, m_hndl[id]);
}

//�Đ��J�n���Ԑݒ�
void SoundManager::SetVolume(tagSoundID id, float volume)
{
	//DxLib�̈�����0�`255�̊ԂȂ̂ŁA�����v�Z�����Ă���
	ChangeVolumeSoundMem((int)(255.0f * volume), m_hndl[id]);
}