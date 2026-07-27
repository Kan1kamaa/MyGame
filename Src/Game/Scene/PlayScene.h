#pragma once
#include"Scenebase.h"
#include"../Camera/CameraManager.h"
#include"../Object/Field/Sky.h"
#include "../Object/Field/Field.h"
#include"../Object/Player/CharacterManager.h"
#include"../Object/Enemy/EnemyManager.h"
#include"../System/GameCollision.h"
#include"../System/SoundManager.h"
class PlayScene : public SceneBase {
private:
	//�v���C���[
	CharacterManager player;
	//�e
	ShotManager shot;
	//�G
	EnemyManager enemy;
	//�w�i
	Field field;
	Sky sky;
	//�J����
	CameraManager camera;

public:
	//�`��
	void Draw();

private:
	//������
	void Init();
	//���[�h
	void Load();
	//���t���[���v�Z���鏈��
	void Step();
	//���X�V
	void Update();
	//�j��
	void Fin();
};