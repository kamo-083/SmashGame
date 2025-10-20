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
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// 大きさ
	DirectX::SimpleMath::Vector3 m_startScale;	// 初期
	DirectX::SimpleMath::Vector3 m_nowScale;	// 現在

	// 寿命
	float m_life;

	// 色
	DirectX::SimpleMath::Color m_color;

public:
	// コンストラクタ
	ParticleUtility(
		DirectX::SimpleMath::Vector3 startPosition,
		DirectX::SimpleMath::Vector3 scale,
		float life,
		DirectX::SimpleMath::Color color);

	// デストラクタ
	~ParticleUtility() = default;

	// 更新
	bool Update(float elapsedTime);

	// 座標の取得
	const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
	// 大きさの取得
	const DirectX::SimpleMath::Vector3 GetScale() { return m_nowScale; }
	// 寿命の取得
	const float GetLife() { return m_life; }
	// 色の取得
	const DirectX::SimpleMath::Color GetColor() { return m_color; }

};