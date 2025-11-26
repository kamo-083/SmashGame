/**
* @file   PhysicsObject.h
*
* @brief  物理演算に関するヘッダファイル
*/

// 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
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

	static constexpr float ANGULAR_VELOCITY_DAMPING = 0.9f;	// 角速度の減衰率
	static constexpr float ANGULAR_VELOCITY_MAX = 50.0f;	// 角速度の上限

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

	// 角速度
	DirectX::SimpleMath::Vector3 m_angularVelocity;
	// 慣性
	DirectX::SimpleMath::Vector3 m_inertia;


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
	DirectX::SimpleMath::Vector3 GetAngVelocity() { return m_angularVelocity; }		// 角速度

	// デバッグフォントの描画
	void DrawDebugFont(DebugFont* debugFont, int y);

	// 接地面情報のリセット
	void ResetGroundInfo();

	// 法線が地面かどうかを取得
	static bool IsGroundNormal(const DirectX::SimpleMath::Vector3 normal);

	// 接地面情報の設定
	void SetGroundInfo(const DirectX::SimpleMath::Vector3 normal);

	// 接触面の法線ベクトルを取得
	const DirectX::SimpleMath::Vector3& GetGroundNormal() { return m_groundNormal; }

	// 地面に振れているかを取得
	bool IsOnGround() { return m_onGround; }

	// 角速度の加算
	void AddAngVelocity(DirectX::SimpleMath::Vector3 angVel);

private:
	// 反射ベクトルの計算
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(
		DirectX::SimpleMath::Vector3 velocity,
		DirectX::SimpleMath::Vector3 normal);

	// 角速度の減衰
	void DampingAngularVelocity();

	// 角速度の制限
	void ClampAngularVelocity();
};

