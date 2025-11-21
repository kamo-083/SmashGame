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

	static constexpr float GROUND_COS_THRESHOLD = 30.0f;	// 地面判定用の最大傾斜角の閾値

	static constexpr int DEBUG_FONT_INTERVAL = 25;	// デバッグ用フォントの表示間隔

	// 重力
	Gravity m_gravity;

	// 外力
	ExternalForce m_externalForce;

	// 摩擦力
	Friction m_friction;

	// 地面に触れているか
	bool m_onGround;
	// 接触面の法線ベクトル
	DirectX::SimpleMath::Vector3 m_groundNormal;

public:
	// コンストラクタ・デストラクタ
	PhysicsObject();
	~PhysicsObject() = default;

	// 速度に加える力の計算
	void CalculateForce(
		DirectX::SimpleMath::Vector3& velocity,
		float mass,
		float elapsedTime);

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

	// 接地面情報のリセット
	void ResetGroundInfo();

	// 接地面情報の設定
	void SetGroundInfo(DirectX::SimpleMath::Vector3 const normal);

	// 接触面の法線ベクトルを取得
	const DirectX::SimpleMath::Vector3& GetGroundNormal() { return m_groundNormal; }

	// 地面に振れているかを取得
	bool IsOnGround() { return m_onGround; }

private:
	// 反射ベクトルの計算
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(
		DirectX::SimpleMath::Vector3 velocity,
		DirectX::SimpleMath::Vector3 normal);
};

