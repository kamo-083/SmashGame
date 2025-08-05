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
EffectManager::EffectManager()
{

}



/**
 * @brief デストラクタ
 */
EffectManager::~EffectManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Initialize()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Update()
{

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Draw()
{

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

TrajectoryParticle* EffectManager::CreateTrajectory()
{
	m_trajectory.push_back(std::make_unique<TrajectoryParticle>());
	//m_trajectory[m_trajectory.size()]->Create(

	//)

	return nullptr;
}
