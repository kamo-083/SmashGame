#include "pch.h"
#include "PhysicsObject.h"

using namespace DirectX;

PhysicsObject::PhysicsObject()
	: m_gravity{}
	, m_externalForce{}
	, m_friction{}
{
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::CalculateForce(DirectX::SimpleMath::Vector3& velocity, 
								   float mass,
								   float elapsedTime,
								   bool onGround)
{
	SimpleMath::Vector3 totalForce = SimpleMath::Vector3::Zero;

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
	SimpleMath::Vector3 acceleration = totalForce / mass;

	//加速度の反映
	velocity += acceleration * elapsedTime;

	//力のリセット
	m_externalForce.Reset();
}


void PhysicsObject::Reflection(DirectX::SimpleMath::Vector3& velocity,
							   DirectX::SimpleMath::Vector3& normal, 
							   float restitution)
{
	DirectX::SimpleMath::Vector3 v = velocity;
	DirectX::SimpleMath::Vector3 n = normal;
	DirectX::SimpleMath::Vector3 reflectionVector = CalculateReflectionVector(v, n);
	velocity = reflectionVector * restitution; 
}

void PhysicsObject::RollDown(DirectX::SimpleMath::Vector3& velocity, 
							 DirectX::SimpleMath::Vector3& normal, 
							 float mass,
							 float radius,
							 float elapsedTime)
{
	SimpleMath::Vector3 gravity = { 0.0f,-m_gravity.Get(),0.0f };
	SimpleMath::Vector3 gTangent = gravity - normal * gravity.Dot(normal);
	SimpleMath::Vector3 acceleration = (5.0f / 7.0f) * gTangent;

	velocity += acceleration * elapsedTime;
}

void PhysicsObject::DrawDebugFont(Imase::DebugFont* debugFont, float y)
{
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"gravity = %f", m_gravity.Get());
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"friction = %f", m_friction.Get());
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"externalForce = %f,%f,%f", m_externalForce.Get().x, m_externalForce.Get().y, m_externalForce.Get().z);
}

DirectX::SimpleMath::Vector3 PhysicsObject::CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
																      DirectX::SimpleMath::Vector3 normal)
{
	return velocity - 2.0f * (velocity * normal) * normal;
}