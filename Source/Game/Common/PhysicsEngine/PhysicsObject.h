/**
* @file   PhysicsObject.h
*
* @brief  物理演算に関するヘッダファイル
*/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/PhysicsEngine/Gravity.h"
#include"Source/Game/Common/PhysicsEngine/ExternalForce.h"
#include"Source/Game/Common/PhysicsEngine/Friction.h"

#include"ImaseLib/DebugFont.h"


class PhysicsObject
{
private:
	static constexpr float RESTITUTION_COEFFICIENT = 1.0f;	// 反発係数のデフォルト値

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
		float mass,
		float radius,
		float elapsedTime);

	// 取得
	ExternalForce& GetExternalForce() { return m_externalForce; }	// 外力
	Gravity& GetGravity() { return m_gravity; }						// 重力
	Friction& GetFriction() { return m_friction; }					// 摩擦力

	// デバッグフォントの描画
	void DrawDebugFont(Imase::DebugFont* debugFont, float y);

private:
	// 反射ベクトルの計算
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
														   DirectX::SimpleMath::Vector3 normal);
};