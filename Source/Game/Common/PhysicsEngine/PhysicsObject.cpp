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
 * @param onGround		対象が地面と接しているか
 *
 * @return なし
 */
void PhysicsObject::CalculateForce(
	DirectX::SimpleMath::Vector3& velocity,
	float mass,
	float elapsedTime,
	bool onGround)
{
	DirectX::SimpleMath::Vector3 totalForce = DirectX::SimpleMath::Vector3::Zero;

	//外力
	m_externalForce.Calculate(totalForce);

	//摩擦力(接地時)
	if (onGround)
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

	//力のリセット
	m_externalForce.Reset();
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
	DirectX::SimpleMath::Vector3 v = velocity;
	DirectX::SimpleMath::Vector3 n = normal;
	DirectX::SimpleMath::Vector3 reflectionVector = CalculateReflectionVector(v, n);
	velocity = reflectionVector * restitution;
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
	DirectX::SimpleMath::Vector3 gravity = { 0.0f,-m_gravity.Get(),0.0f };
	DirectX::SimpleMath::Vector3 gTangent = gravity - normal * gravity.Dot(normal);
	DirectX::SimpleMath::Vector3 acceleration = (5.0f / 7.0f) * gTangent;

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