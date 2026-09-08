#include"Field.h"



//ロード
void Field::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Field/Field_grassland.mv1");
	}
}
