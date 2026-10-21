#include "EffectManager.h"
#include "EffekseerForDXLib.h"
#include <math.h>

namespace {
	//エフェクト1種類ぶんの設定
	struct EffectInfo {
		const char* path;		//エフェクトファイルのパス
		float forwardDist;		//基準座標から向いている方向へずらす距離
		float height;			//基準座標から上へずらす高さ
		float scale;			//拡大率(Effekseerの1単位を、このゲームの大きさに合わせる)
		float rotOffsetY;		//向きの補正(エフェクトの正面とキャラの正面がずれているとき用)
	};

	//EffectId と同じ並びで書くこと
	const EffectInfo EFFECT_INFO[EffectManager::EFFECT_NUM] = {
		{ "Data/Effect/SwordAttack/SwordAttack.efkefc", 12.0f, 10.0f, 5.0f, DX_PI_F },	//EFFECT_ATTACK
		{ "Data/Effect/SwordSkill/SwordSkill.efkefc",   30.0f,  0.0f, 5.0f, DX_PI_F },	//EFFECT_SKILL
		{ "Data/Effect/SwordUlt/SwordUlt.efkefc",        0.0f,  0.0f, 5.0f, DX_PI_F },	//EFFECT_ULT
	};
}

//コンストラクタ
EffectManager::EffectManager()
{
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		m_effectHndl[i] = -1;
	}
}

//デストラクタ
EffectManager::~EffectManager()
{
	Fin();
}

//初期化
void EffectManager::Init()
{
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		m_effectHndl[i] = -1;
	}
}

//エフェクトファイルのロード
void EffectManager::Load()
{
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		if (m_effectHndl[i] == -1)
		{
			m_effectHndl[i] = LoadEffekseerEffect(EFFECT_INFO[i].path);
		}
	}
}

//毎フレーム更新
void EffectManager::Update()
{
	UpdateEffekseer3D();
}

//描画
void EffectManager::Draw()
{
	//DXライブラリのカメラ設定をEffekseerへ反映してから描画する
	Effekseer_Sync3DSetting();
	DrawEffekseer3D();
}

//破棄
void EffectManager::Fin()
{
	for (int i = 0; i < EFFECT_NUM; i++)
	{
		if (m_effectHndl[i] != -1)
		{
			DeleteEffekseerEffect(m_effectHndl[i]);
			m_effectHndl[i] = -1;
		}
	}
}

//エフェクトを再生する
int EffectManager::Play(EffectId id, const VECTOR& pos, float rotY)
{
	if (id < 0 || id >= EFFECT_NUM)
	{
		return -1;
	}
	if (m_effectHndl[id] == -1)
	{
		return -1;
	}

	const EffectInfo& info = EFFECT_INFO[id];

	//キャラの向いている方向(CharacterManagerの前方ベクトルと同じ取り方)へずらした位置に出す
	VECTOR forward = { -sinf(rotY), 0.0f, -cosf(rotY) };
	VECTOR playPos = VAdd(pos, VScale(forward, info.forwardDist));
	playPos.y += info.height;

	int playingHndl = PlayEffekseer3DEffect(m_effectHndl[id]);
	if (playingHndl == -1)
	{
		return -1;
	}
	SetPosPlayingEffekseer3DEffect(playingHndl, playPos.x, playPos.y, playPos.z);
	SetRotationPlayingEffekseer3DEffect(playingHndl, 0.0f, rotY + info.rotOffsetY, 0.0f);
	SetScalePlayingEffekseer3DEffect(playingHndl, info.scale, info.scale, info.scale);
	return playingHndl;
}
