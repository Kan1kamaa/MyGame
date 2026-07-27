#pragma once
#include"Scenebase.h"
#include"../Camera/CameraManager.h"
#include"../Object/Field/Sky.h"
#include "../Object/Field/Field.h"
#include"../Object/Player/Player.h"
#include"../Object/Enemy/EnemyManager.h"
#include"../System/GameCollision.h"
#include"../System/SoundManager.h"
class PlayScene : public SceneBase {
private:
	//ƒvƒŒƒCƒ„[
	Player player;
	//’e
	ShotManager shot;
	//“G
	EnemyManager enemy;
	//”wŒi
	Field field;
	Sky sky;
	//ƒJƒƒ‰
	CameraManager camera;

public:
	//•`‰æ
	void Draw();

private:
	//‰Šú‰»
	void Init();
	//ƒ[ƒh
	void Load();
	//–ˆƒtƒŒ[ƒ€ŒvZ‚·‚éˆ—
	void Step();
	//î•ñXV
	void Update();
	//”jŠü
	void Fin();
};