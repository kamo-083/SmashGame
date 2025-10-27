/**
 * @file   ParticleUtility.h
 *
 * @brief  パーティクルユーティリティに関するヘッダファイル
 */

#pragma once

#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/DeviceResources.h"

class ParticleUtility
{
public:
	struct EffectParam
	{
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Color color;
	};

	struct ParticleData
	{
		EffectParam start;
		EffectParam end;
		float life;
	};

private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 寿命
	float m_life;

	// 大きさ
	DirectX::SimpleMath::Vector3 m_scale;

	// 色
	DirectX::SimpleMath::Color m_color;

	// パラメータ一覧
	ParticleData m_params;

public:
	// コンストラクタ
	ParticleUtility(
		DirectX::SimpleMath::Vector3 position,
		ParticleData params);

	// デストラクタ
	~ParticleUtility() = default;

	// 更新
	bool Update(float elapsedTime);

	// 座標の取得
	const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 寿命の取得
	const float GetLife() { return m_life; }
	// 大きさの取得
	const DirectX::SimpleMath::Vector3 GetScale() { return m_scale; }
	// 色の取得
	const DirectX::SimpleMath::Color GetColor() { return m_color; }
};