#include "JiggleBone.h"
#include "math.h"

//コンストラクタ
JiggleBone::JiggleBone() : m_frameIndex(-1), m_phase(0.0f),
m_preset{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
m_angleX(0.0f), m_angleZ(0.0f), m_velocityX(0.0f), m_velocityZ(0.0f)
{
	m_restMatrix = MGetIdent();
}

//ボーンを名前で探して初期化する
bool JiggleBone::Init(int modelHandle, const char* frameName, float phase, const JigglePreset& preset)
{
	m_frameIndex = MV1SearchFrame(modelHandle, frameName);
	m_phase = phase;
	m_preset = preset;
	m_angleX = 0.0f;
	m_angleZ = 0.0f;
	m_velocityX = 0.0f;
	m_velocityZ = 0.0f;

	//見つからなければ、これ以降Updateで何もしないようにして終わる
	if (m_frameIndex == -1)
	{
		return false;
	}

	//ボーン本来の姿勢(親からの位置・向き)を覚えておく
	//(MV1SetFrameUserLocalMatrixは行列を丸ごと置き換えるので、これが無いと揺れを反映した瞬間に
	//ボーンが親の原点に潰れてしまう)
	m_restMatrix = MV1GetFrameLocalMatrix(modelHandle, m_frameIndex);

	return true;
}

//毎フレームの物理計算とボーンへの反映
void JiggleBone::Update(int modelHandle, VECTOR localMoveDelta, float turnDelta, float time, float animPower,
	float biasRight, float biasForward)
{
	//見つかっていないボーンは何もしない
	if (m_frameIndex == -1)
	{
		return;
	}

	//待機中の揺れ(サイン波)。m_phaseの分だけボーンごとにタイミングをずらす
	float idleX = sinf(time * m_preset.idleSpeed + m_phase) * m_preset.idleAmplitude;
	float idleZ = cosf(time * m_preset.idleSpeed + m_phase) * m_preset.idleAmplitude;

	//移動・旋回に反応する分の目標角度(動いた方向と逆向きに傾かせる)
	float reactX = -localMoveDelta.z * m_preset.moveReactPower;
	float reactZ = localMoveDelta.x * m_preset.moveReactPower - turnDelta * m_preset.turnReactPower;

	//脚から押し出したい分の角度(足に近づきすぎている時だけ0以外になる)
	float biasX = -biasForward;
	float biasZ = biasRight;

	//待機の揺れと反応分にアニメーションの倍率を掛け、押し出し分はそのまま足したものが目標角度
	//(押し出しは「見た目の演出」ではなく「貫通を避ける」ためのものなので、animPowerでは弱めない)
	float targetX = (idleX + reactX) * animPower + biasX;
	float targetZ = (idleZ + reactZ) * animPower + biasZ;

	//バネ+減衰で目標角度に近づける
	m_velocityX += (targetX - m_angleX) * m_preset.springPower;
	m_velocityX *= m_preset.springDamping;
	m_angleX += m_velocityX;

	m_velocityZ += (targetZ - m_angleZ) * m_preset.springPower;
	m_velocityZ *= m_preset.springDamping;
	m_angleZ += m_velocityZ;

	//計算した傾きを回転行列にして、ボーン本来の姿勢に掛け合わせてから反映する
	//(傾きの回転 → 本来の姿勢、の順で掛け合わせる)
	MATRIX swayMatrix = MMult(MGetRotX(m_angleX), MGetRotZ(m_angleZ));
	MV1SetFrameUserLocalMatrix(modelHandle, m_frameIndex, MMult(swayMatrix, m_restMatrix));
}
