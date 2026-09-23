#include"Shot.h"
#include"../../System/SoundManager.h"
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };
static const float MOVE_RANGE = 2000.0f;	//�ړ��\�͈�
static const float SHOT_RAD = 5.0f;
static const float SHOT_ATTACK_POWER = 15.0f;	//プレイヤーの弾1発あたりの攻撃力
//�R���X�g���N�^
Shot::Shot() :m_speed(VEC_ZERO)
{
}

//�f�X�g���N�^
Shot::~Shot()
{
	Fin();
}

//������
void Shot::Init()
{
	ObjectBase::Init();
	m_radius = SHOT_RAD;
	m_speed = VEC_ZERO;
	m_isActive = false;		//�ŏ��͌����Ȃ��悤�ɏ����Ă���
}

//���[�h
void Shot::Load(int origiinhndl)
{
	if (m_hndl == -1)
	{
		//���f���͕�������
		m_hndl = MV1DuplicateModel(origiinhndl);
	}
}

//���t���[���v�Z���鏈��
void Shot::Step()
{
	//�t���O�I�t�Ȃ�I��
	if (m_isActive == false)return;

	//���݂̍��W�ɑ��x���v�Z
	m_pos = VAdd(m_pos, m_speed);
	//���͈͂𒴂��������
	if (m_pos.x < -MOVE_RANGE || m_pos.x > MOVE_RANGE
		|| m_pos.z < -MOVE_RANGE || m_pos.z > MOVE_RANGE)
	{
		m_isActive = false;
	}
}

//�V���b�g����
bool Shot::Request(const VECTOR& pos, const VECTOR& speed)
{
	//���ɔ��˂���Ă�����I��
	if (m_isActive == true)return false;

	m_pos = pos;
	m_speed = speed;
	m_isActive = true;
	SoundManager::Play(SoundManager::SE_PLSHOT);
	return true;
}

float Shot::GetAttackPower() const
{
	return SHOT_ATTACK_POWER;
}