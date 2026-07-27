#include"Collsion.h"

//---------------------------------
// 点と四角の当たり判定
//---------------------------------
bool Collsion::ChekHitDotToSquare(VECTOR dotPos, VECTOR squarePos,
	int width, int height)
{
	// 四角形の上下左右それぞれの座標を計算する
	float up = squarePos.y - height * 0.5f;
	float down = squarePos.y + height * 0.5f;
	float left = squarePos.x - width * 0.5f;
	float right = squarePos.x + width * 0.5f;

	// 4つの端をそれぞれチェックして、すべての条件を満たしたらヒット！
	if (dotPos.x >= left && dotPos.x <= right
		&& dotPos.y >= up && dotPos.y <= down)
	{
		return true;
	}
	else return false;
}

//---------------------------------
// 円同士の当たり判定
//---------------------------------
bool Collsion::CheckHitCircleToCircle(VECTOR circlePos1, float radius1,
	VECTOR circlePos2, float radius2)
{
	// 円1から円2までの距離を計算
	float lengthX = circlePos1.x - circlePos2.x;	// どうせ2乗するとマイナスが消えるので、順番はどうでもいい
	lengthX *= lengthX;
	float lengthY = circlePos1.y - circlePos2.y;	// どうせ2乗するとマイナスが消えるので、順番はどうでもいい
	lengthY *= lengthY;
	float length = lengthX + lengthY;	// これがaの2乗＋bの2乗

	// 2つの円の半径を加算し、2乗する
	float lengthRadius = (float)(radius1 + radius2);
	lengthRadius *= lengthRadius;


	// 以下の条件を満たせばヒットする！
	if (lengthRadius >= length)
	{
		return true;
	}
	else return false;
}

//---------------------------------
// 矩形同士の当たり判定
//---------------------------------
bool Collsion::CheckHitSquareToSquare(VECTOR squarePos1, int width1, int height1,
	VECTOR squarePos2, int width2, int height2)
{
	// 四角形の上下左右それぞれの座標を計算する
	float up1 = squarePos1.y - height1 * 0.5f;
	float down1 = squarePos1.y + height1 * 0.5f;
	float left1 = squarePos1.x - width1 * 0.5f;
	float right1 = squarePos1.x + width1 * 0.5f;

	float up2 = squarePos2.y - height2 * 0.5f;
	float down2 = squarePos2.y + height2 * 0.5f;
	float left2 = squarePos2.x - width2 * 0.5f;
	float right2 = squarePos2.x + width2 * 0.5f;

	// 4つの端をそれぞれチェックして、すべての条件を満たしたらヒット！
	if (left1 <= right2 && right1 >= left2
		&& up1 <= down2 && down1 >= up2)
	{
		return true;
	}
	else return false;
}



//---------------------------------
// 箱同士の当たり判定
//---------------------------------
bool Collsion::CheckHitSphereToSphere(VECTOR Sphere1, float size1,
	VECTOR Sphere2, float size2)
{
	//三平方の定理
	float circlerangeX = Sphere1.x - Sphere2.x;
	circlerangeX *= circlerangeX;
	float circlerangeY = Sphere1.y - Sphere2.y;
	circlerangeY *= circlerangeY;
	float circlerangeZ = Sphere1.z - Sphere2.z;
	circlerangeZ *= circlerangeZ;
	//それぞれの半径を足して2乗
	float hankei = size1 + size2;
	hankei *= hankei;
	if (circlerangeX + circlerangeY + circlerangeZ < hankei)
	{

		return true;

	}
	else return false;
}
bool CheckHitBoxToBox(VECTOR boxPos1, VECTOR size1,
	VECTOR boxPos2, VECTOR size2)
{
	// 箱の上下左右奥手前それぞれの座標を計算する
	float up1 = boxPos1.y + size1.y * 0.5f;
	float down1 = boxPos1.y - size1.y * 0.5f;
	float left1 = boxPos1.x - size1.x * 0.5f;
	float right1 = boxPos1.x + size1.x * 0.5f;
	float front1 = boxPos1.z - size1.z * 0.5f;
	float back1 = boxPos1.z + size1.z * 0.5f;

	float up2 = boxPos2.y + size2.y * 0.5f;
	float down2 = boxPos2.y - size2.y * 0.5f;
	float left2 = boxPos2.x - size2.x * 0.5f;
	float right2 = boxPos2.x + size2.x * 0.5f;
	float front2 = boxPos2.z - size2.z * 0.5f;
	float back2 = boxPos2.z + size2.z * 0.5f;

	// 4つの端をそれぞれチェックして、すべての条件を満たしたらヒット！
	if (left1 <= right2 && right1 >= left2
		&& up1 >= down2 && down1 <= up2)
	{
		// 最後に奥行きもチェック
		if (front1 <= back2 && back1 >= front2)
		{
			return true;
		}
	}

	return false;
}
