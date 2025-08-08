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
	for (std::unique_ptr<TrajectoryParticleData>& trajectry : m_trajectory)
	{
		trajectry->effect->Update(elapsedTime, *trajectry->position, trajectry->spawn, trajectry->random);
		trajectry->effect->CreateBillboard(*trajectry->position, m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp());
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
	for (std::unique_ptr<TrajectoryParticleData>& trajectry : m_trajectory)
	{
		trajectry->effect->Render(m_pCamera->GetView(), proj);
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
	const wchar_t* texPath, float scale, float life, DirectX::SimpleMath::Color color, DirectX::SimpleMath::Vector3* position, bool random)
{
	// 軌跡エフェクトの作成
	std::unique_ptr<TrajectoryParticle> effect = std::make_unique<TrajectoryParticle>();
	effect->Create(
		m_pDeviceResources, texPath, scale, life, color
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
