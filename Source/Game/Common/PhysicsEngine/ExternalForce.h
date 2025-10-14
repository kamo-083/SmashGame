/**
 * @file   ExternalForce.h
 *
 * @brief  外力に関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"pch.h"



// クラスの定義 ===============================================================
/**
 * @brief 外力
 */
class ExternalForce
{
private:
	DirectX::SimpleMath::Vector3 m_externalForceValue;

public:
	//コンストラクタ
	ExternalForce()
		:m_externalForceValue{ DirectX::SimpleMath::Vector3::Zero }
	{

	}

	//外力の加算
	void Calculate(DirectX::SimpleMath::Vector3& force)
	{
		force += m_externalForceValue;
	}

	void Set(DirectX::SimpleMath::Vector3 force)
	{
		m_externalForceValue = force;
	}

	void Add(DirectX::SimpleMath::Vector3 force)
	{
		m_externalForceValue += force;
	}

	void Reset()
	{
		m_externalForceValue = DirectX::SimpleMath::Vector3::Zero;
	}

	DirectX::SimpleMath::Vector3 Get()
	{
		return m_externalForceValue;
	}
};