#include"Sky.h"



//ロード
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

//更新(天球をプレイヤーの位置に追従させる)
void Sky::Update(VECTOR followPos)
{
	//高さ(y)は動かさず、水平方向(x, z)だけプレイヤーに合わせる
	//(天球はどの高さから見ても同じように見えるモデルなので、高さまで合わせる必要はない)
	m_pos.x = followPos.x;
	m_pos.z = followPos.z;

	MV1SetPosition(m_hndl, m_pos);
	MV1SetRotationXYZ(m_hndl, m_rot);
	MV1SetScale(m_hndl, m_scale);
}
