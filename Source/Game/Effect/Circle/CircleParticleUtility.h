//--------------------------------------------------------------------------------------
// File: CircleParticleUtility.h
//-------------------------------------------------------------------------------------

#pragma once

#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

class CircleParticleUtility
{
private:
	//座標
	DirectX::SimpleMath::Vector3 m_position;

	//大きさ
	DirectX::SimpleMath::Vector3 m_startScale;
	DirectX::SimpleMath::Vector3 m_nowScale;

	//寿命
	float m_life;

	//色
	DirectX::SimpleMath::Color m_color;

public:
	//コンストラクタ
	CircleParticleUtility(
		DirectX::SimpleMath::Vector3 startPosition,
		DirectX::SimpleMath::Vector3 scale,
		float life,
		DirectX::SimpleMath::Color color);

	//デストラクタ
	~CircleParticleUtility();

	//更新
	bool Update(float elapsedTime);

	//取得
	const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }

	const DirectX::SimpleMath::Vector3 GetScale() { return m_nowScale; }

	const float GetLife() { return m_life; }

	const DirectX::SimpleMath::Color GetColor() { return m_color; }

};