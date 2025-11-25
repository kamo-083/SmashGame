/**
 * @file   PhysicsObject.cpp
 *
 * @brief  物理演算に関するソースファイル
 */


// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PhysicsObject.h"

/**
 * @brief コンストラクタ
 */
PhysicsObject::PhysicsObject()
	: m_gravity{}
	, m_externalForce{}
	, m_friction{}
	, m_onGround{ false }
	, m_groundNormal{ DirectX::SimpleMath::Vector3::UnitY }
{
}


/**
 * @brief 速度に加える力の計算
 * 
 * 外力、摩擦力、重力を加える
 *
 * @param velocity		対象の速度
 * @param mass			対象の質量
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void PhysicsObject::CalculateForce(
	DirectX::SimpleMath::Vector3& velocity,
	float mass,
	float elapsedTime)
{
	//角速度の減衰
	DampingAngularVelocity();

	if (mass == 0.0f) return;	// 0除算防止

	DirectX::SimpleMath::Vector3 totalForce = DirectX::SimpleMath::Vector3::Zero;

	//外力
	m_externalForce.Calculate(totalForce);

	//摩擦力(接地時)
	if (m_onGround)
	{
		m_friction.ApplyToForce(totalForce, mass * m_gravity.Get());
		m_friction.ApplyToVelocity(velocity, mass * m_gravity.Get(), elapsedTime);
	}

	//重力
	m_gravity.Calculate(totalForce, mass);

	//加速度の計算
	DirectX::SimpleMath::Vector3 acceleration = totalForce / mass;

	//加速度の反映
	velocity += acceleration * elapsedTime;

	//リセット
	m_externalForce.Reset();
	ResetGroundInfo();
}


/**
 * @brief 跳ね返り処理
 *
 * @param velocity		対象の速度
 * @param normal		跳ね返る面の法線
 * @param restitution	反発係数
 *
 * @return なし
 */
void PhysicsObject::Reflection(
	DirectX::SimpleMath::Vector3& velocity,
	DirectX::SimpleMath::Vector3& normal,
	float restitution)
{
	using vector3 = DirectX::SimpleMath::Vector3;

	if (normal == vector3::Zero) normal = m_groundNormal;

	vector3 v = velocity;
	vector3 n = normal;
	vector3 reflectionVector = CalculateReflectionVector(v, n);
	velocity = reflectionVector * restitution;

	// 角速度の加算
	AddAngVelocity(n);
}


/**
 * @brief 転がり処理
 *
 * @param velocity		対象の速度
 * @param normal		転がる面の法線
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void PhysicsObject::RollDown(
	DirectX::SimpleMath::Vector3& velocity,
	DirectX::SimpleMath::Vector3& normal,
	float elapsedTime)
{
	using vector3 = DirectX::SimpleMath::Vector3;

	vector3 gravity = { 0.0f,-m_gravity.Get(),0.0f };
	vector3 gTangent = gravity - normal * gravity.Dot(normal);
	vector3 acceleration = (5.0f / 7.0f) * gTangent;

	velocity += acceleration * elapsedTime;
}


/**
 * @brief 表示するデバッグ情報の追加
 *
 * @param debugFont	デバッグフォントのポインタ
 * @param y			表示するY座標
 *
 * @return なし
 */
void PhysicsObject::DrawDebugFont(DebugFont* debugFont, int y)
{
	y += DEBUG_FONT_INTERVAL;
	debugFont->AddString(0, y, DirectX::Colors::White, L"gravity = %f", m_gravity.Get());
	y += DEBUG_FONT_INTERVAL;
	debugFont->AddString(0, y, DirectX::Colors::White, L"friction = %f", m_friction.Get());
	y += DEBUG_FONT_INTERVAL;
	debugFont->AddString(0, y, DirectX::Colors::White, L"externalForce = %f,%f,%f", m_externalForce.Get().x, m_externalForce.Get().y, m_externalForce.Get().z);
}


/**
 * @brief 接地面情報のリセット
 *
 * @param なし
 *
 * @return なし
 */
void PhysicsObject::ResetGroundInfo()
{
	m_onGround = false;
	m_groundNormal = DirectX::SimpleMath::Vector3::UnitY;
}


/**
 * @brief 接地面情報のリセット
 *
 * @param なし
 *
 * @return なし
 */
void PhysicsObject::SetGroundInfo(DirectX::SimpleMath::Vector3 const normal)
{
	const float groundCos = std::cos(DirectX::XMConvertToRadians(GROUND_COS_THRESHOLD));
	if (normal.y >= groundCos)
	{
		m_onGround = true;
		m_groundNormal = normal;
	}
}


/**
 * @brief 角速度の加算
 *
 * @param angVel	加算する値
 *
 * @return なし
 */
void PhysicsObject::AddAngVelocity(DirectX::SimpleMath::Vector3 angVel)
{
	m_angularVelocity += angVel;
	ClampAngularVelocity();
}


/**
 * @brief 反射ベクトルの計算
 *
 * @param velocity		対象の速度
 * @param normal		転がる面の法線
 *
 * @return なし
 */
DirectX::SimpleMath::Vector3 PhysicsObject::CalculateReflectionVector(
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 normal)
{
	return velocity - 2.0f * (velocity * normal) * normal;
}


/**
 * @brief 角速度の減衰
 *
 * @param なし
 *
 * @return なし
 */
void PhysicsObject::DampingAngularVelocity()
{
	if (m_angularVelocity == DirectX::SimpleMath::Vector3::Zero) return;

	m_angularVelocity *= ANGULAR_VELOCITY_DAMPING;

	// 各値が0.01を下回ったら0にする
	if (m_angularVelocity.x < 0.01f) m_angularVelocity.x = 0.0f;
	if (m_angularVelocity.y < 0.01f) m_angularVelocity.y = 0.0f;
	if (m_angularVelocity.z < 0.01f) m_angularVelocity.z = 0.0f;
}


/**
 * @brief 角速度の制限
 *
 * @param なし
 *
 * @return なし
 */
void PhysicsObject::ClampAngularVelocity()
{
	if (m_angularVelocity.x > ANGULAR_VELOCITY_MAX) m_angularVelocity.x = ANGULAR_VELOCITY_MAX;
	if (m_angularVelocity.y > ANGULAR_VELOCITY_MAX) m_angularVelocity.y = ANGULAR_VELOCITY_MAX;
	if (m_angularVelocity.z > ANGULAR_VELOCITY_MAX) m_angularVelocity.z = ANGULAR_VELOCITY_MAX;
}
