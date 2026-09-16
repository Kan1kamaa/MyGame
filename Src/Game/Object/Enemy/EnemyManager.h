#pragma once
#include"Enemy.h"

//�G�̐��̍ő�l
static const int ENEMY_MAX = 50;

class EnemyManager {
private:
	Enemy m_Enemy[ENEMY_MAX];		//�G
	int m_waitCnt;					//���̓G���o��܂ł̎���
	int m_EnemyCnt;
public:
	//�R���X�g���N�^�E�f�X�g���N�^
	EnemyManager();
	~EnemyManager();
	
	//������
	void Init();
	//���[�h
	void Load();
	//���t���[���v�Z���鏈��
	void Step(const VECTOR& playerPos);
	//���X�V
	void Update();
	//�`��
	void Draw();
	//�j��
	void Fin();

	
	//@pos : ���˂�����W
	//@speed : �ړ����x
	//@return : true = �������� false = ���s
	bool RequestEnemy();
	//�ʂ̓G�f�[�^�擾
	Enemy& GetEnemy(int index) { return m_Enemy[index]; }
};