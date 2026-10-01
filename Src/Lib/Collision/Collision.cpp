#include"Collsion.h"

//球同士の当たり判定(3D)
bool Collsion::CheckHitSphereToSphere(VECTOR pos1, float radius1, VECTOR pos2, float radius2)
{
	//2つの球の中心間の距離が、半径の合計以下なら当たっている
	float radiusSum = radius1 + radius2;
	VECTOR diff = VSub(pos1, pos2);
	float distSq = VDot(diff, diff);

	return distSq <= radiusSum * radiusSum;
}
