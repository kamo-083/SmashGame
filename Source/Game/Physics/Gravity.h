/**
 * @file   Gravity.h
 *
 * @brief  重力に関するヘッダファイル
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
	static constexpr float GLOBAL_GRAVITY = 9.8f;	// 重力のデフォルト値

	// 重力の値
	float m_gravityValue;

public:
	/**
	* @brief コンストラクタ
	*
	* @param gravityValue	重力
	*/
	Gravity(float gravityValue = GLOBAL_GRAVITY)
		: m_gravityValue{ gravityValue }
	{

	}

	
	/**
	* @brief デストラクタ
	*/
	~Gravity() = default;


	/**
	* @brief 対象に重力を加算(対力)
	*
	* @param force	加える対象
	*
	* @return なし
	*/
	void Calculate(DirectX::SimpleMath::Vector3& force, float mass)
	{
		force.y -= mass * m_gravityValue;
	}


	/**
	* @brief 対象に重力を加算(対座標)
	*
	* @param force	加える対象
	*
	* @return なし
	* 
	*/
	void Apply(DirectX::SimpleMath::Vector3& pos, float elapsedTime)
	{
		pos.y -= m_gravityValue * elapsedTime;
	}

		
	/**
	* @brief 重力の値を取得
	*
	* @param なし
	*
	* @return 重力の値
	* 
	*/
	float Get()
	{
		return m_gravityValue;
	}
};