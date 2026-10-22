#include"Field.h"
#include"../../System/ModelUtil.h"



//ロード
void Field::Load()
{
	if (m_modelHandle == -1)
	{
		m_modelHandle = MV1LoadModel("Data/Models/Field/Field_grassland.mv1");
		BrightenModelAmbient(m_modelHandle);
	}
}

//指定したXZ座標の地面の高さを返す(今は常に平面なので0.0f固定)
float Field::GetGroundHeight(float x, float z)
{
	return 0.0f;
}
