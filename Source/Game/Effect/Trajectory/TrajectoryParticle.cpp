/**
 * @file   TrajectoryParticle.cpp
 *
 * @brief  TrajectoryParticleに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TrajectoryParticle.h"
#include <random>

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TrajectoryParticle::TrajectoryParticle()
	: IEffectParticle{}
{

}



/**
 * @brief デストラクタ
 */
TrajectoryParticle::~TrajectoryParticle()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TrajectoryParticle::Update(float elapsedTime, DirectX::SimpleMath::Vector3 pos, bool spawn, bool isRandom)
{
	//パーティクルの生成
	m_timer += elapsedTime;
	if (m_timer > 0.1f && spawn)
	{
		if (isRandom)
		{
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<> random(0.0f, XM_2PI);
			std::uniform_real_distribution<> random2(-1.0f, 1.0f);

			float angle = random(mt);
			float range = random2(mt);

			float x = cosf(angle) * range;
			float z = sinf(angle) * range;
			pos += SimpleMath::Vector3(x, 0.0f, z);
		}

		//座標の設定
		ParticleUtility particleUtility{
			pos,
			SimpleMath::Vector3(m_scale,m_scale,m_scale),
			m_life,
			m_color
		};
		m_particleUtility.push_back(particleUtility);	//ビルボードしたい

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
