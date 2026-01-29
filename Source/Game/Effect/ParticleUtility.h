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
		float life = 0.0f;
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
		const DirectX::SimpleMath::Vector3& position,
		const ParticleData& params);

	// デストラクタ
	~ParticleUtility() = default;

	// 更新
	bool Update(float elapsedTime);

	// 座標の取得
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 寿命の取得
	float GetLife() const { return m_life; }
	// 大きさの取得
	DirectX::SimpleMath::Vector3 GetScale() const { return m_scale; }
	// 色の取得
	DirectX::SimpleMath::Color GetColor() const { return m_color; }
};