/**
 * @file   ExternalForce.h
 *
 * @brief  外力に関するヘッダファイル
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
	// 外力の値
	DirectX::SimpleMath::Vector3 m_externalForceValue;

public:
	/**
	* @brief コンストラクタ
	*
	* @param なし
	*/	
	ExternalForce()
		:m_externalForceValue{ DirectX::SimpleMath::Vector3::Zero }
	{

	}

	
	/**
	* @brief デストラクタ
	*/
	~ExternalForce() = default;


	/**
	* @brief 対象に外力を加算
	*
	* @param force	加える対象
	*
	* @return なし
	*/
	void Calculate(DirectX::SimpleMath::Vector3& force)
	{
		force += m_externalForceValue;
	}


	/**
	* @brief 外力の設定
	*
	* @param force	設定値
	*
	* @return なし
	*/
	void Set(DirectX::SimpleMath::Vector3 force)
	{
		m_externalForceValue = force;
	}


	/**
	* @brief 外力の加算
	*
	* @param force	加算値
	*
	* @return なし
	*/
	void Add(DirectX::SimpleMath::Vector3 force)
	{
		m_externalForceValue += force;
	}


	/**
	* @brief 外力のリセット
	*
	* @return なし
	*/
	void Reset()
	{
		m_externalForceValue = DirectX::SimpleMath::Vector3::Zero;
	}


	/**
	* @brief 外力の値を取得
	*
	* @return 外力の値
	*/
	DirectX::SimpleMath::Vector3 Get()
	{
		return m_externalForceValue;
	}
};