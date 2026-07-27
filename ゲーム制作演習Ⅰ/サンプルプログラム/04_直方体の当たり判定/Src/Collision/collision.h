#pragma once

#include <DxLib.h>

// 点と四角の当たり判定
//	dotPos		:	点の座標
//	squarePos	:	四角形の中心座標
//	width		:	四角形の横幅
//	height		:	四角形の縦幅
//	@return		:	true=当たった　false=当たっていない
bool ChekHitDotToSquare(VECTOR dotPos, VECTOR squarePos,
	int width, int height);

// 矩形同士の当たり判定
//	squarePos	:	矩形の中心座標
//	width		:	矩形の横幅
//	height		:	矩形の縦幅
//	@return		:	true=当たった　false=当たっていない
bool CheckHitSquareToSquare(VECTOR squarePos1, int width1, int height1,
	VECTOR squarePos2, int width2, int height2);

// 円同士の当たり判定
//	circlePos	:	円の中心座標
//	radius		:	円の半径
//	@return		:	true=当たった　false=当たっていない
bool CheckHitCircleToCircle(VECTOR circlePos1, int radius1,
	VECTOR circlePos2, int radius2);



