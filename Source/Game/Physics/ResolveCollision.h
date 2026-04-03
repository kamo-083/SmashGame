/**
* @file   ResolveCollision.h
*
* @brief  衝突判定のめり込み解決に関するヘッダファイル
*/

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/CollisionManager.h"

static constexpr float POSITION_CORRECTION_RATE = 0.6f; // めり込み補正率(1フレームあたり)
static constexpr float PENETRATION_SLOP = 0.05f;        // 許容めり込み量(振動防止)

// 球vsOBB
void ResolveSphereVsOBB(CollisionManager::Node& a, CollisionManager::Node& b);

// 球vs球
void ResolveSphereVsSphere(CollisionManager::Node& a, CollisionManager::Node& b);

// OBBvsOBB
void ResolveOBBVsOBB(CollisionManager::Node& a, CollisionManager::Node& b);

// 速度をスライド
void SlideVelocity(
	DirectX::SimpleMath::Vector3* velocity,
	const DirectX::SimpleMath::Vector3& normal);