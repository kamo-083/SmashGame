/**
 * @file   ParticleUtility.cpp
 *
 * @brief  パーティクルユーティリティに関するソースファイル
 */

#include "pch.h"
#include "ParticleUtility.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param position  出現位置
 * @param scale		大きさ
 * @param life		寿命
 * @param color		色
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
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
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
