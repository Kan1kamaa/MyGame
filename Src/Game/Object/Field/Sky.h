#pragma once
#include<DxLib.h>
#include"../ObjectBase/ObjectBase.h"
//地面を表示
class Sky :public ObjectBase {
private:

public:
	void Load();
	void Step();
	//更新
	//@followPos : 天球の中心にしたい座標(プレイヤーの位置を渡す)
	//(天球をワールド原点に固定したままだと、プレイヤーが原点から離れた時に
	//天球の端までの距離がカメラの描画距離(Far)を超えてしまい、黒い隙間が見えてしまう。
	//これを防ぐため、天球を常にプレイヤーの真上あたりに追従させる)
	void Update(VECTOR followPos);
};
