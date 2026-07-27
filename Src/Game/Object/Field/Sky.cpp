#include"Sky.h"



//ÉçÅ[Éh
void Sky::Load()
{
	if (m_hndl == -1)
	{
		m_hndl = MV1LoadModel("Data/Models/Sky/Sky.MV1");
	}
}
void Sky::Step()
{
	m_rot.y+= 0.00005;
}