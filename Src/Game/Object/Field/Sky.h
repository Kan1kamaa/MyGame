#pragma once
#include<DxLib.h>
#include"../ObjectBase/ObjectBase.h"
//地面を表示
class Sky :public ObjectBase {
private:

public:
	void Load();
	void Step();
	//更新
};
