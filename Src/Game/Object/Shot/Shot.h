#pragma once
#include"../ObjectBase/ObjectBase.h"

class Shot : public ObjectBase {
private:
	VECTOR m_speed;		//�ړ����x
public:
	//�R���X�g���N�^�E�f�X�g���N�^
	Shot();
	~Shot();

	//������
	void Init();
	//���[�h
	void Load(int originhndl);
	//���t���[���v�Z���鏈��
	void Step();
	//�V���b�g����
	//@pos : ���˂�����W
	//@sped :�@�ړ����x
	//@return : true = �������� false = ���s
	bool Request(const VECTOR& pos, const VECTOR& speed);
	//�����蔻��̒��S���擾
	VECTOR GetCollisionPos(){ return m_pos; }
	//�U����
	float GetAttackPower() const override;
};