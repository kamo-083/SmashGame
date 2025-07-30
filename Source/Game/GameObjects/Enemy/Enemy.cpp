/**
 * @file   Enemy.cpp
 *
 * @brief  敵に関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Enemy.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Enemy::Enemy()
	:m_rotY{0.0f}
	,m_onGround{false}
	,m_isAttack{false}
	,m_attackForce{0.0f}
	,m_collisionType{OBBCollider::CollisionType::Others}
{

}



/**
 * @brief デストラクタ
 */
Enemy::~Enemy()
{

}
