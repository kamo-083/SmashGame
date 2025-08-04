//--------------------------------------------------------------------------------------
// File: TrajectoryParticleUtility.cpp
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "TrajectoryParticleUtility.h"

#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include "TrajectoryParticleUtility.h"

using namespace DirectX;

TrajectoryParticleUtility::TrajectoryParticleUtility(
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Vector3 scale,
	float life,
	DirectX::SimpleMath::Color color)
{
	m_position = position;
	m_nowScale = m_startScale = scale;
	m_life = life;
	m_color = color;
}

TrajectoryParticleUtility::~TrajectoryParticleUtility()
{
}

bool TrajectoryParticleUtility::Update(float elapsedTime)
{
	//大きさの更新
	m_nowScale = SimpleMath::Vector3::Lerp(SimpleMath::Vector3::Zero, m_startScale, m_life);
	
	//寿命の更新
	m_life -= elapsedTime;
	if (m_life < 0.0f)
	{
		return false;
	}
	return true;
}
