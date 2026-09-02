#pragma once
#include <DxLib.h>

//揺れものボーン1本分の物理パラメータ(パーツごとに重さ・硬さを変えるための設定一式)
struct JigglePreset
{
	float idleAmplitude;  //待機中の揺れの大きさ(ラジアン)
	float idleSpeed;      //待機中の揺れの速さ
	float springPower;    //バネの強さ(目標角度に近づく速さ。大きいほど機敏)
	float springDamping;  //減衰(1に近いほど揺れが長く続く)
	float moveReactPower; //移動量に対する反応の強さ
	float turnReactPower; //回転量に対する反応の強さ
};

//1本の揺れものボーンの物理(バネ+減衰)を担当するクラス
class JiggleBone {
private:
	int m_frameIndex;    //このボーンのフレーム番号(見つからなければ-1)
	MATRIX m_restMatrix; //ボーン本来の姿勢(親からの位置・向き。初期状態で1回だけ取得する)
	float m_phase;       //待機中の揺れをボーンごとにずらすための位相
	JigglePreset m_preset; //このボーン専用の物理パラメータ

	float m_angleX;     //現在の傾き(X軸)
	float m_angleZ;     //現在の傾き(Z軸)
	float m_velocityX;  //角速度(X軸)
	float m_velocityZ;  //角速度(Z軸)

public:
	//コンストラクタ
	JiggleBone();

	//ボーンを名前で探して初期化する。見つかればtrueを返す
	//@modelHandle : キャラのモデルハンドル
	//@frameName   : 探すボーン名
	//@phase       : 待機中の揺れの位相(ボーンごとに少しずつずらす値)
	//@preset      : このボーンに使う物理パラメータ
	bool Init(int modelHandle, const char* frameName, float phase, const JigglePreset& preset);

	//見つかったボーンかどうか
	bool IsFound() const { return m_frameIndex != -1; }

	//このボーンのフレーム番号(脚との当たり判定など、外から座標を調べたい時に使う)
	int GetFrameIndex() const { return m_frameIndex; }

	//毎フレームの物理計算とボーンへの反映
	//@modelHandle    : キャラのモデルハンドル
	//@localMoveDelta : キャラの向き基準に変換した、このフレームの移動量(x=右方向、z=前方向)
	//@turnDelta      : このフレームで回転した角度
	//@time           : 経過時間(待機中の揺れに使う)
	//@animPower      : 今再生中のアニメーションに応じた揺れの強さの倍率(1.0が基準)
	//@biasRight      : 脚との当たり判定などで追加したい、右方向への角度の押し出し量
	//@biasForward    : 同じく前方向への角度の押し出し量
	void Update(int modelHandle, VECTOR localMoveDelta, float turnDelta, float time, float animPower,
		float biasRight, float biasForward);
};
