/**
 * @file   ParticleUtility.cpp
 *
 * @brief  ParticleUtilityに関するソースファイル
 *
 * @author 清水まこと
 */

#include "pch.h"
#include "ParticleUtility.h"

#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include "ParticleUtility.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] position  出現位置
 * @param[in] scale		大きさ
 * @param[in] life		寿命
 * @param[in] color		色
 */
ParticleUtility::ParticleUtility(
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


/**
 * @brief デストラクタ
 */
ParticleUtility::~ParticleUtility()
{
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
bool ParticleUtility::Update(float elapsedTime)
{
	//大きさの更新
	m_nowScale = DirectX::SimpleMath::Vector3::Lerp(DirectX::SimpleMath::Vector3::Zero, m_startScale, m_life);
	
	//寿命の更新
	m_life -= elapsedTime;
	if (m_life < 0.0f)
	{
		return false;
	}
	return true;
}
