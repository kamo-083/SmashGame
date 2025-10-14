/**
 * @file   Friction.h
 *
 * @brief  外力に関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"pch.h"


using namespace DirectX;

// クラスの定義 ===============================================================
/**
 * @brief 摩擦力
 */
class Friction
{
private:
	static constexpr float GLOBAL_DYNAMIC_FLICTION = 0.95f;
	static constexpr float GLOBAL_STATIC_FLICTION = 1.0f;
	static constexpr float EPSILON = 0.001f;

	float m_dynamicFriction;
	float m_staticFriction;
	bool m_moving;

public:
	//コンストラクタ
	Friction(float dynamicF = GLOBAL_DYNAMIC_FLICTION, float staticF = GLOBAL_STATIC_FLICTION)
		:m_dynamicFriction{ dynamicF }
		,m_staticFriction{ staticF }
		,m_moving{false}
	{

	}

	//摩擦力の加算(外力)
	void ApplyToForce(DirectX::SimpleMath::Vector3& force, float N)
	{
		// 力が極端に小さい場合は０にする
		if (force.Length() < EPSILON)
		{
			force = SimpleMath::Vector3::Zero;
			return;
		}

		float staticLimit = m_staticFriction * N;
		if (!m_moving && staticLimit >= force.Length())
		{
			// 静止摩擦力
			force =SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
		}
		else if (staticLimit < force.Length())
		{
			// 最大静止摩擦力を超えていたら移動状態にする
			m_moving = true;
		}

		// 動いていたら
		if (m_moving)
		{
			// 動摩擦力
			SimpleMath::Vector3 frictionDir = -force;
			frictionDir.Normalize();

			SimpleMath::Vector3 friction = N * m_dynamicFriction * frictionDir;
			force += friction;
		}

		if (force.Length() == 0.0f)
		{
			// 静止状態にする
			m_moving = false;
		}
	}

	// 摩擦力の計算(移動速度)
	void ApplyToVelocity(DirectX::SimpleMath::Vector3& velocity, float N, float elapsedTime)
	{
		// 速度が極端に小さい場合は０にする
		if (velocity.Length() < EPSILON)
		{
			velocity = SimpleMath::Vector3::Zero;
			return;
		}

		SimpleMath::Vector3 frictionDir = -velocity;
		frictionDir.Normalize();

		SimpleMath::Vector3 friction = N * m_dynamicFriction * frictionDir * elapsedTime;

		// 摩擦力より移動速度が小さい場合は０にする
		if (friction.Length() >= velocity.Length())
		{
			velocity = SimpleMath::Vector3::Zero;
		}
		else
		{
			velocity += friction;
		}
	}

	void SetDynamicFriction(float dynamicF)
	{
		m_dynamicFriction = dynamicF;
	}

	void SetStaticFriction(float staticF)
	{
		m_staticFriction = staticF;
	}

	void Reset()
	{
		m_dynamicFriction = GLOBAL_DYNAMIC_FLICTION;
	}

	float Get()
	{
		return m_dynamicFriction;
	}
};