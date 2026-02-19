/**
 * @file   Friction.h
 *
 * @brief  摩擦力に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"pch.h"


// クラスの定義 ===============================================================
/**
 * @brief 摩擦力
 */
class Friction
{
private:
	static constexpr float GLOBAL_DYNAMIC_FLICTION = 0.95f;	// 動摩擦係数のデフォルト値
	static constexpr float GLOBAL_STATIC_FLICTION = 1.0f;	// 静止摩擦係数のデフォルト値
	static constexpr float EPSILON = 0.001f;				// 極端に小さな値

	// 動摩擦係数
	float m_dynamicFriction;
	// 静止摩擦係数
	float m_staticFriction;
	// 移動状態フラグ
	bool m_moving;

public:
	/**
	* @brief コンストラクタ
	*
	* @param dynamicF 動摩擦係数
	* @param staticF  静止摩擦係数
	*/
	Friction(float dynamicF = GLOBAL_DYNAMIC_FLICTION, float staticF = GLOBAL_STATIC_FLICTION)
		:
		m_dynamicFriction{ dynamicF },
		m_staticFriction{ staticF },
		m_moving{false}
	{

	}


	/**
	* @brief デストラクタ
	*/
	~Friction() = default;


	/**
	* @brief 外力に対して摩擦力を加算
	*
	* @param force　外力のベクトル
	* @param N	　	垂直抗力
	*	
	* @return なし
	*/
	void ApplyToForce(DirectX::SimpleMath::Vector3& force, float N)
	{
		// 力が極端に小さい場合は0にする
		if (force.Length() < EPSILON)
		{
			force = DirectX::SimpleMath::Vector3::Zero;
			return;
		}

		float staticLimit = m_staticFriction * N;
		if (!m_moving && staticLimit >= force.Length())
		{
			// 静止摩擦力
			force = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
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
			DirectX::SimpleMath::Vector3 frictionDir = -force;
			frictionDir.Normalize();

			DirectX::SimpleMath::Vector3 friction = N * m_dynamicFriction * frictionDir;
			force += friction;
		}

		if (force.Length() == 0.0f)
		{
			// 静止状態にする
			m_moving = false;
		}
	}


	/**
	* @brief 移動に対して摩擦力を加算
	*
	* @param velocity　 移動速度のベクトル
	* @param N	　		垂直抗力
	*
	* @return なし
	*/
	void ApplyToVelocity(DirectX::SimpleMath::Vector3& velocity, float N, float elapsedTime)
	{
		// 速度が極端に小さい場合は０にする
		if (velocity.Length() < EPSILON)
		{
			velocity = DirectX::SimpleMath::Vector3::Zero;
			return;
		}

		DirectX::SimpleMath::Vector3 frictionDir = -velocity;
		frictionDir.Normalize();

		DirectX::SimpleMath::Vector3 friction = N * m_dynamicFriction * frictionDir * elapsedTime;

		// 摩擦力より移動速度が小さい場合は０にする
		if (friction.Length() >= velocity.Length())
		{
			velocity = DirectX::SimpleMath::Vector3::Zero;
		}
		else
		{
			velocity += friction;
		}
	}

	
	/**
	* @brief 動摩擦係数を設定
	*
	* @param dynamicF	設定値
	*	
	* @return なし
	*/
	void SetDynamicFriction(float dynamicF)
	{
		m_dynamicFriction = dynamicF;
	}


	/**
	* @brief 静止摩擦係数を設定
	*
	* @param staticF	設定値
	*
	* @return なし
	*/
	void SetStaticFriction(float staticF)
	{
		m_staticFriction = staticF;
	}


	/**
	* @brief 動摩擦係数を基本値にリセット
	*
	* @param なし
	*
	* @return なし
	*/
	void Reset()
	{
		m_dynamicFriction = GLOBAL_DYNAMIC_FLICTION;
	}


	/**
	* @brief 動摩擦係数を取得
	*
	* @param なし
	*
	* @return 動摩擦係数
	*/
	float Get()
	{
		return m_dynamicFriction;
	}
};