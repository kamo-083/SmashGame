/**
 * @file   Gravity.h
 *
 * @brief  重力に関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"pch.h"



// クラスの定義 ===============================================================
/**
 * @brief 重力
 */
class Gravity
{
private:
	static constexpr float GLOBAL_GRAVITY = 9.8f;

	float m_gravityValue;

public:
	//コンストラクタ
	Gravity(float gravityValue = GLOBAL_GRAVITY)
		:m_gravityValue{ gravityValue }
	{

	}

	//重力の加算
	void Calculate(DirectX::SimpleMath::Vector3& force, float mass)
	{
		force.y -= mass * m_gravityValue;
	}

	//重力の加算(対座標)
	void Apply(DirectX::SimpleMath::Vector3& pos, float elapsedTime)
	{
		pos.y -= m_gravityValue * elapsedTime;
	}

	float Get()
	{
		return m_gravityValue;
	}
};