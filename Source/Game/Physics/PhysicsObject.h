/**
* @file   PhysicsObject.h
*
* @brief  物理演算に関するヘッダファイル
*/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/Gravity.h"
#include"Source/Game/Physics/ExternalForce.h"
#include"Source/Game/Physics/Friction.h"

#include"Source/Debug/DebugFont.h"


class PhysicsObject
{
private:
	static constexpr float RESTITUTION_COEFFICIENT = 1.0f;	// 反発係数のデフォルト値

	static constexpr int DEBUG_FONT_INTERVAL = 25;	// デバッグ用フォントの表示間隔

	// 重力
	Gravity m_gravity;

	// 外力
	ExternalForce m_externalForce;

	// 摩擦力
	Friction m_friction;

public:
	// コンストラクタ・デストラクタ
	PhysicsObject();
	~PhysicsObject() = default;

	// 速度に加える力の計算
	void CalculateForce(
		DirectX::SimpleMath::Vector3& velocity,
		float mass,
		float elapsedTime,
		bool onGround);

	// 跳ね返り処理
	void Reflection(
		DirectX::SimpleMath::Vector3& velocity,
		DirectX::SimpleMath::Vector3& normal,
		float restitution = RESTITUTION_COEFFICIENT);

	// 転がり処理
	void RollDown(
		DirectX::SimpleMath::Vector3& velocity,
		DirectX::SimpleMath::Vector3& normal,
		float elapsedTime);

	// 取得
	ExternalForce& GetExternalForce() { return m_externalForce; }	// 外力
	Gravity& GetGravity() { return m_gravity; }						// 重力
	Friction& GetFriction() { return m_friction; }					// 摩擦力

	// デバッグフォントの描画
	void DrawDebugFont(DebugFont* debugFont, int y);

private:
	// 反射ベクトルの計算
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
														   DirectX::SimpleMath::Vector3 normal);
};