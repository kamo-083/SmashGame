/**
 * @file   TrajectoryParticle.cpp
 *
 * @brief  軌跡エフェクトパーティクルに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TrajectoryParticle.h"
#include <random>


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
TrajectoryParticle::TrajectoryParticle()
	: IEffectParticle{}
{

}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 * @param pos			座標
 * @param spawn			出現させるか
 * @param isRandom		出現位置にランダム性を持たせるか
 *
 * @return なし
 */
void TrajectoryParticle::Update(float elapsedTime, const DirectX::SimpleMath::Vector3& pos, bool spawn, bool isRandom)
{
	DirectX::SimpleMath::Vector3 position = pos;

	//パーティクルの生成
	m_timer += elapsedTime;
	if (m_timer > 0.1f && spawn)
	{
		if (isRandom)
		{
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<float> random(0.0f, DirectX::XM_2PI);
			std::uniform_real_distribution<float> random2(-RANDOM_RANGE, RANDOM_RANGE);

			float angle = random(mt);
			float range = random2(mt);

			float x = cosf(angle) * range;
			float z = sinf(angle) * range;
			position += DirectX::SimpleMath::Vector3(x, 0.0f, z);
		}

		//設定
		ParticleUtility particleUtility{
			position,
			m_particleData
		};
		m_particleUtility.push_back(particleUtility);

		m_timer = 0.0f;
	}

	//パーティクルの更新
	for (std::list<ParticleUtility>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{
		if (!(ite)->Update(elapsedTime))
		{
			//	falseが返ってきたら、消す
			ite = m_particleUtility.erase(ite);

			if (ite == m_particleUtility.end())
			{
				break;
			}
		}
	}
}
