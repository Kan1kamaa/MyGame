#include "ObjectBase.h"

//VECTOR構造体の中身を0にするために使用する
static const VECTOR VEC_ZERO{ 0.0f,0.0f,0.0f };

//コンストラクタ
ObjectBase::ObjectBase() :m_pos(VEC_ZERO),
m_rot(VEC_ZERO), m_scale(VEC_ZERO),m_hndl(-1), m_isActive(false)
{
}

//デストラクタ
ObjectBase::~ObjectBase()
{
	Fin();
}

//初期化
void ObjectBase::Init()
{
	m_pos = VEC_ZERO;
	m_rot = VEC_ZERO;
	m_scale = { 1.0f,1.0f,1.0f };
	m_radius = 0.0f;
	m_isActive = true;
}

//ロード
void ObjectBase::Load()
{

}

//毎フレーム更新する処理
void ObjectBase::Step()
{

}

//情報更新
void ObjectBase::Update()
{
	MV1SetPosition(m_hndl, m_pos);
	MV1SetRotationXYZ(m_hndl, m_rot);
	MV1SetScale(m_hndl, m_scale);
}

//描画
void ObjectBase::Draw()
{
	if (!m_isActive)return;

	MV1DrawModel(m_hndl);


}

//破棄
void ObjectBase::Fin()
{
	if (m_hndl != -1)
	{
		MV1DeleteModel(m_hndl);
		m_hndl = -1;
	}
}

void ObjectBase::HitCalc(const ObjectBase& other)
{
	m_isActive = false;
}