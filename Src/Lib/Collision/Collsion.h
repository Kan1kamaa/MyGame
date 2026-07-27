#pragma once
#include"DxLib.h"

class Collsion {
public:
	//点と四角の当たり判定
	static bool ChekHitDotToSquare(VECTOR pos1,VECTOR pos2,int w, int h);
	//円同士の当たり判定
	static bool CheckHitCircleToCircle(VECTOR pos1,float radius1,VECTOR pos2,float radius2);
	//短形同士の当たり判定(3D)
	//@x      : 横の位置
	//@y      : 縦の位置
	//@w      : 横幅(半分のサイズ)
	//@h      : 縦幅(半分のサイズ)
	static bool CheckHitSquareToSquare(VECTOR pos1,  int w1, int h1, VECTOR pos2, int w2, int h2);

	//箱同士の当たり判定(3D)
	//@pos    : 箱の中心座標
	//@size   : 箱のサイズ(半分サイズ)
	static bool CheckHitBoxToBox(VECTOR pos1, VECTOR size1, VECTOR pos2, VECTOR size2);

	//球同士の当たり判定(3D)
	//@pos    ; 球の中心座標
	//@radius : 球の半径
	static bool CheckHitSphereToSphere(VECTOR pos1, float radius1, VECTOR pos2, float radius2);

};
