#include "ObjectBase.h"

//VECTOR�\���̂̒��g��0�ɂ��邽�߂Ɏg�p����
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };

//�R���X�g���N�^
ObjectBase::ObjectBase() :m_pos(VEC_ZERO),
m_rot(VEC_ZERO), m_scale(VEC_ZERO),m_hndl(-1), m_isActive(false)
{
}

//�f�X�g���N�^
ObjectBase::~ObjectBase()
{
	Fin();
}

//������
void ObjectBase::Init()
{
	m_pos = VEC_ZERO;
	m_rot = VEC_ZERO;
	m_scale = { 1.0f,1.0f,1.0f };
	m_radius = 0.0f;
	m_isActive = true;
}

//���[�h
void ObjectBase::Load()
{

}

//���t���[���X�V���鏈��
void ObjectBase::Step()
{

}

//���X�V
void ObjectBase::Update()
{
	MV1SetPosition(m_hndl, m_pos);
	MV1SetRotationXYZ(m_hndl, m_rot);
	MV1SetScale(m_hndl, m_scale);
}

//�`��
void ObjectBase::Draw()
{
	if (m_isActive == false)return;

	MV1DrawModel(m_hndl);


}

//�j��
void ObjectBase::Fin()
{
	if (m_hndl != -1)
	{
		MV1DeleteModel(m_hndl);
		m_hndl = -1;
	}
}

void ObjectBase::HitCalc(const ObjectBase& other)
{
	m_isActive = false;
}