/**
 * @file   CircleParticle.cpp
 *
 * @brief  CircleParticleに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CircleParticle.h"
#include<random>


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CircleParticle::CircleParticle()
	: IEffectParticle{}
{

}



/**
 * @brief デストラクタ
 */
CircleParticle::~CircleParticle()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
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
 * @brief エフェクトを出現
 *
 * @param[in] pos		   出現位置
 * @param[in] range		   円の半径
 * @param[in] isRandom	   位置のランダム性
 * @param[in] isHorizontal 円の向き
 * @param[in] num		   1度に出現する数
 *
 * @return なし
 */
void CircleParticle::SpawnParticleEffect(
	DirectX::SimpleMath::Vector3 pos,
	float range,
	bool isRandom,
	bool isHorizontal,
	int num)
{
	if (m_timer < SPAWN_INTERVAL) return;

	for (int i = 0; i < num; i++)
	{
		float angle = 0.0f;
		float scale = m_scale;
		if (isRandom)
		{
			//乱数生成器
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<> random(0.0f, DirectX::XM_2PI);
			std::uniform_real_distribution<> random2(-0.5f, 0.5f);

			angle = random(mt);
			scale += random2(mt);
		}
		else
		{
			angle = DirectX::XM_2PI * static_cast<float>(i) / static_cast<float>(num);
		}

		DirectX::SimpleMath::Vector3 transPos = pos;
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

		//座標の設定
		ParticleUtility particleUtility{
			transPos,
			DirectX::SimpleMath::Vector3(scale,scale,scale),
			m_life,
			m_color
		};
		m_particleUtility.push_back(particleUtility);
	}

	m_timer = 0.0f;
}