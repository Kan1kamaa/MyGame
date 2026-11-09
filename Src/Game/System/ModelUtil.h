#pragma once
#include <DxLib.h>

//モデルのマテリアルに設定するアンビエント色の明るさ(0.0〜1.0)
//ライティング有効時、環境光はマテリアルのアンビエント色に掛かるので、暗いときはここを上げる
static const float MODEL_AMBIENT_BRIGHTNESS = 0.5f;

//モデルの全マテリアルのアンビエント色を明るくする(モデルを読み込んだ直後に呼ぶ)
inline void BrightenModelAmbient(int modelHndl)
{
	if (modelHndl == -1)
	{
		return;
	}

	int materialNum = MV1GetMaterialNum(modelHndl);
	for (int i = 0; i < materialNum; i++)
	{
		MV1SetMaterialAmbColor(modelHndl, i,
			GetColorF(MODEL_AMBIENT_BRIGHTNESS, MODEL_AMBIENT_BRIGHTNESS, MODEL_AMBIENT_BRIGHTNESS, 1.0f));
	}
}

//輪郭線の太さ(モデルの座標系での幅)。遠近法で見えるので、遠くの敵ほど自然に細く見える
static const float MODEL_OUTLINE_WIDTH = 0.1f;

//モデルの全マテリアルに輪郭線を設定する(モデルを読み込んだ直後に呼ぶ)
//敵やプレイヤーとマップの境界を見やすくするために使う
//@modelHndl : 輪郭線を付けるモデル
//@r, g, b   : 輪郭線の色(0.0〜1.0)
inline void SetModelOutline(int modelHndl, float r, float g, float b)
{
	if (modelHndl == -1)
	{
		return;
	}

	int materialNum = MV1GetMaterialNum(modelHndl);
	for (int i = 0; i < materialNum; i++)
	{
		MV1SetMaterialOutLineColor(modelHndl, i, GetColorF(r, g, b, 1.0f));
		MV1SetMaterialOutLineWidth(modelHndl, i, MODEL_OUTLINE_WIDTH);
	}
}
