#pragma once
#include <vector>
#include "../ObjectBase/ObjectBase.h"
#include "FairyBase.h"

//プレイヤーの精霊を管理するクラス
//今ついてくる精霊(1体)と、切り替え後に攻撃を出し切るまで残る精霊(複数)を持つ
class FairyManager : public ObjectBase {
private:
	//残しておける切り替え済みの精霊の最大数(超えたら一番古いものを消す)
	static const int MAX_LEAVING = 3;

	FairyBase* m_active;				//今ついてくる精霊(無ければnullptr)
	std::vector<FairyBase*> m_leaving;	//切り替え済みで攻撃を出し切るまで残っている精霊
	FairyElement m_element;				//今の属性
	int m_offsetIdx;					//追従位置の番号

	//前フレームのキー入力状態(押した瞬間だけ発動させるための判定用)
	bool m_prevKeyE;		//攻撃
	bool m_prevKeyR;		//追従位置の切り替え
	bool m_prevKey1;		//風
	bool m_prevKey2;		//炎
	bool m_prevKey3;		//雷

	//属性に合った精霊を生成して初期化する
	//@startPos : 出現させる座標
	FairyBase* CreateFairy(FairyElement element, VECTOR startPos);
	//精霊を片付けて削除する
	void DestroyFairy(FairyBase* fairy);
	//現在の追従位置番号に対応するオフセットを返す
	VECTOR GetOffset() const;
	//キー入力を読んで、攻撃・属性切り替え・位置切り替えをする
	void ReadInput();
	//属性を切り替える。今の精霊は攻撃を出し切るまで残り、新しい精霊が出る
	void SwapElement(FairyElement element);
	//追従位置を次の位置に変える
	void CycleOffset();

public:
	//コンストラクタ・デストラクタ
	FairyManager();
	~FairyManager();

	//初期化(最初の精霊を出す)
	//@playerPos : プレイヤーの座標
	void Init(VECTOR playerPos);
	//毎フレーム計算する処理
	//@playerPos : プレイヤーの座標
	void Step(VECTOR playerPos);
	//毎更新
	void Update();
	//精霊のモデル描画(影用の描画にも使う)
	void DrawFairy();
	//破棄
	void Fin();
	//リスポーンなどで精霊を最初の状態に戻す
	void Reset(VECTOR playerPos);

	//=========================
	//精霊の攻撃当たり判定(GameCollisionから使う)
	//ついてくる精霊+残っている精霊を、0〜GetFairyCount()-1 で順に取り出して
	//それぞれの IsAttackActive / GetAttackPos / GetAttackRadius / GetAttackPower を使う
	//=========================
	int GetFairyCount() const;
	FairyBase* GetFairy(int index) const;
};
