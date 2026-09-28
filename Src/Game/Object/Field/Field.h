#pragma once
#include<DxLib.h>
#include"../ObjectBase/ObjectBase.h"
//�n�ʂ�\��
class Field :public ObjectBase{
private:

public:
	void Load();
	//�X�V

	//指定したXZ座標の地面の高さを返す(今は常に平面=0.0fだが、
	//地形に高低差をつけるときはここだけ直せば済むようにしている)
	static float GetGroundHeight(float x, float z);
};