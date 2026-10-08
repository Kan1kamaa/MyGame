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
