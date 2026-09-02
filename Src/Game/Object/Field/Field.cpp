#include"Field.h"



//���[�h
void Field::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Field/Field.mv1");
	}
}
