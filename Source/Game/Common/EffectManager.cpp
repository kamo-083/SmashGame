/**
 * @file   EffectManager.cpp
 *
 * @brief  エフェクトマネージャーに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EffectManager.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EffectManager::EffectManager(DX::DeviceResources* deviceResources)
	: m_pDeviceResources{ deviceResources }
	, m_pCamera{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
EffectManager::~EffectManager()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Update(float elapsedTime)
{
	// 軌跡エフェクトの更新
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Update(elapsedTime, *trajectory->position, trajectory->spawn, trajectory->random);
		trajectory->effect->CreateBillboard(*trajectory->position, m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp());
	}

	// 円形エフェクトの更新
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Update(elapsedTime);
		circle->effect->CreateBillboard(*circle->position, m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp());
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Draw(DirectX::SimpleMath::Matrix proj)
{
	// 軌跡エフェクトの描画
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Render(m_pCamera->GetView(), proj);
	}

	// 円形エフェクトの描画
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Render(m_pCamera->GetView(), proj);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Finalize()
{

}

EffectManager::TrajectoryParticleData* EffectManager::CreateTrajectory(
	ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color,
	DirectX::SimpleMath::Vector3* position, bool random)
{
	// 軌跡エフェクトの作成
	std::unique_ptr<TrajectoryParticle> effect = std::make_unique<TrajectoryParticle>();
	effect->Create(
		m_pDeviceResources, texture, scale, life, color
	);

	// パーティクルデータ構造体の作成
	std::unique_ptr<TrajectoryParticleData> data = std::make_unique<TrajectoryParticleData>
		(
			std::move(effect),
			position,
			false,
			random
		);

	// 配列に格納
	m_trajectory.push_back(std::move(data));

	// 作成したエフェクトのポインタを返す
	return m_trajectory.back().get();
}

EffectManager::CircleParticleData* EffectManager::CreateCircle(
	ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color, 
	DirectX::SimpleMath::Vector3* position, float range, int num, bool random, bool horizontal)
{
	// 円形エフェクトの作成
	std::unique_ptr<CircleParticle> effect = std::make_unique<CircleParticle>();
	effect->Create(
		m_pDeviceResources, texture, scale, life, color
	);

	// パーティクルデータ構造体の作成
	std::unique_ptr<CircleParticleData> data = std::make_unique<CircleParticleData>
		(
			std::move(effect),
			position,
			range,
			num,
			random,
			horizontal
		);

	// 配列に格納
	m_circle.push_back(std::move(data));

	// 作成したエフェクトのポインタを返す
	return m_circle.back().get();
}
