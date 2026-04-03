/**
 * @file   CircleParticle.cpp
 *
 * @brief  円形エフェクトパーティクルに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CircleParticle.h"
#include<random>

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
CircleParticle::CircleParticle()
	: IEffectParticle{}
{
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void CircleParticle::Update(float elapsedTime)
{
	m_timer += elapsedTime;

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

/**
 * @brief エフェクトを発生させる
 *
 * @param pos		   出現位置
 * @param range		   円の半径
 * @param isRandom	   位置のランダム性
 * @param isHorizontal 円の向き
 * @param num		   1度に出現する数
 *
 * @return なし
 */
void CircleParticle::SpawnParticleEffect(
	const DirectX::SimpleMath::Vector3& pos,
	float range,
	bool isRandom,
	bool isHorizontal,
	int num)
{
	if (m_timer < SPAWN_INTERVAL) return;

	for (int i = 0; i < num; i++)
	{
		float angle = 0.0f;		// 角度
		float scale = m_particleData.start.scale.Length();	// 大きさ
		if (isRandom)
		{
			//乱数生成器
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<float> random(0.0f, DirectX::XM_2PI);
			std::uniform_real_distribution<float> random2(-RANDOM_RANGE, RANDOM_RANGE);

			angle = random(mt);
			scale += random2(mt);
		}
		else
		{
			angle = DirectX::XM_2PI * static_cast<float>(i) / static_cast<float>(num);
		}

		DirectX::SimpleMath::Vector3 transPos = pos;
		// 向き
		if (isHorizontal)
		{
			float x = cosf(angle) * range;
			float z = sinf(angle) * range;
			transPos += DirectX::SimpleMath::Vector3(x, 0.0f, z);
		}
		else
		{
			float y = cosf(angle) * range;
			float x = sinf(angle) * range;
			transPos += DirectX::SimpleMath::Vector3(x, y, 0.0f);
		}

		// パラメータの作成
		ParticleUtility::ParticleData data =
		{
			{DirectX::SimpleMath::Vector3(scale), m_particleData.start.color},
			{m_particleData.end.scale, m_particleData.end.color},
			m_particleData.life
		};

		data.start.scale = m_particleData.start.scale;

		//位置の設定
		ParticleUtility particleUtility{
			transPos,
			data
		};
		m_particleUtility.push_back(particleUtility);
	}

	m_timer = 0.0f;
}