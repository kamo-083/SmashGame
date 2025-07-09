/**
 * @file   Fliction.h
 *
 * @brief  外力に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"pch.h"



// クラスの定義 ===============================================================
/**
 * @brief 摩擦力
 */
class Fliction
{
private:
	static constexpr float GLOBAL_DYNAMIC_FLICTION = 0.95f;
	static constexpr float GLOBAL_STATIC_FLICTION = 1.0f;

	float m_dynamicFlictionForce;
	float m_staticFrictionForce;
	float m_maxStaticFrictionForce;

public:
	//コンストラクタ
	Fliction(float dynamicFliction = GLOBAL_DYNAMIC_FLICTION, float staticFriction = GLOBAL_STATIC_FLICTION)
		:m_dynamicFlictionForce{ dynamicFliction }
		,m_staticFrictionForce{ staticFriction }
		,m_maxStaticFrictionForce{ staticFriction }
	{

	}

	//摩擦力の加算
	void Calculate(DirectX::SimpleMath::Vector3& force, float N)
	{
		float staticF = m_staticFrictionForce * N;
		if (staticF < force.Length())
		{
			// 動摩擦力
			DirectX::SimpleMath::Vector3 v = -force;
			v.Normalize();

			DirectX::SimpleMath::Vector3 flictionForce = N * m_dynamicFlictionForce * v;
			force += flictionForce;
		}
		else
		{
			// 静止摩擦力
			force = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
		}
	}

	void SetDynamicForce(float dynamicF)
	{
		m_dynamicFlictionForce = dynamicF;
	}

	void SetStaticForce(float staticF)
	{
		m_staticFrictionForce = staticF;
	}

	void Reset()
	{
		m_dynamicFlictionForce = GLOBAL_DYNAMIC_FLICTION;
	}

	float Get()
	{
		return m_dynamicFlictionForce;
	}
};