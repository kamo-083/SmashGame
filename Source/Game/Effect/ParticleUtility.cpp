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
 * @param params		パラメータ一覧
 */
ParticleUtility::ParticleUtility(
	DirectX::SimpleMath::Vector3 position,
	ParticleData params)
{
	m_position = position;
	m_scale = params.start.scale;
	m_life = params.life;
	m_color = params.start.color;
	m_params = params;
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
	// 大きさの更新
	m_scale = DirectX::SimpleMath::Vector3::Lerp(m_params.end.scale, m_params.start.scale, m_life);

	// 色の更新
	m_color = DirectX::SimpleMath::Color::Lerp(m_params.end.color, m_params.start.color, m_life);

	// 寿命の更新
	m_life -= elapsedTime;
	if (m_life <= 0.0f)
	{
		return false;
	}
	return true;
}