#pragma once
#include<DxLib.h>
//物体操作の基本クラス
class ObjectBase {
public:
	VECTOR m_pos;		//座標
	VECTOR m_rot;		//拡大角度
	VECTOR m_scale;		//拡大縮小率
	float m_radius;     //当たり判定用半径(球)
	int m_hndl;			//モデルハンドル
	bool m_isActive;	//生存フラグ

public:
	//コンストラクタ・デストラクタ
	ObjectBase();
	~ObjectBase();

	//初期化
	void Init();
	//ロード
	void Load();
	//毎フレーム計算する処理
	void Step();
	//情報更新
	void Update();
	//描画
	void Draw();
	//破棄
	void Fin();

	//各種アクセサ
	//座標
	void SetPos(VECTOR pos) { m_pos = pos; }
	VECTOR GetPos() { return m_pos; }
	//角度
	void SetRot(VECTOR rot) { m_rot = rot; }
	VECTOR GetRot() { return m_rot; }
	//拡大縮小率
	void SetScale(VECTOR scale) { m_scale = scale; }
	VECTOR GetScale() { return m_scale; }
	//生存フラグ
	void SetActive(bool isActive) { m_isActive = isActive; }
	bool GetActive() { return m_isActive; }
	//半径
	float GetRadius() { return m_radius; }
	//当たり判定の中心を取得
	virtual VECTOR GetCollisionPos() { return{ m_pos.x, m_pos.y + m_radius, m_pos.z };}

	virtual void HitCalc(const ObjectBase& other);
};