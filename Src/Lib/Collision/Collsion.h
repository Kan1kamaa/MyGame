#pragma once
#include"DxLib.h"

class Collision {
public:
	//点と四角の当たり判定
	static bool CheckHitDotToSquare(VECTOR pos1,VECTOR pos2,int w, int h);
	//円同士の当たり判定
	static bool CheckHitCircleToCircle(VECTOR pos1,float radius1,VECTOR pos2,float radius2);
	//矩形同士の当たり判定(3D)
	//@pos     : 矩形の中心座標
	//@w,@h   : 横幅・縦幅(半分のサイズ)
	static bool CheckHitSquareToSquare(VECTOR pos1,  int w1, int h1, VECTOR pos2, int w2, int h2);

	//箱同士の当たり判定(3D)
	//@pos    : 箱の中心座標
	//@size   : 箱のサイズ(半分サイズ)
	static bool CheckHitBoxToBox(VECTOR pos1, VECTOR size1, VECTOR pos2, VECTOR size2);

	//球同士の当たり判定(3D)
	//@pos    : 球の中心座標
	//@radius : 球の半径
	static bool CheckHitSphereToSphere(VECTOR pos1, float radius1, VECTOR pos2, float radius2);

};
