#include"Field.h"



//ÉçÅ[Éh
void Field::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Field/Field.pmx");
	}
}
