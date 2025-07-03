#include "pch.h"
#include "PhysicsObject.h"

using namespace DirectX;

PhysicsObject::PhysicsObject()
	: m_gravity{}
	, m_externalForce{}
	, m_fliction{}
{
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::CalculateVelocity(DirectX::SimpleMath::Vector3& velocity, 
									  float mass,
									  bool onGround, 
									  float elapsedTime)
{
	SimpleMath::Vector3 force = SimpleMath::Vector3::Zero;

	//外力
	m_externalForce.Calculate(force);

	//重力
	m_gravity.Calculate(force, mass);

	if (onGround)
	{
		//摩擦力
		m_fliction.Calculate(force, mass * m_gravity.Get());
	}

	//加速度の計算
	SimpleMath::Vector3 acceleration = force / mass;

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

void PhysicsObject::AddFliction(DirectX::SimpleMath::Vector3& velocity, float min)
{
	velocity.x *= m_fliction.Get();
	velocity.z *= m_fliction.Get();

	if (velocity.Length() < min)
	{
		velocity.x = 0.0f;
		velocity.z = 0.0f;
	}
}

void PhysicsObject::DrawDebugFont(Imase::DebugFont* debugFont, float y)
{
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"gravity = %f", m_gravity.Get());
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"fliction = %f", m_fliction.Get());
	y += 25;
	debugFont->AddString(0, y, DirectX::Colors::White, L"externalForce = %f,%f,%f", m_externalForce.Get().x, m_externalForce.Get().y, m_externalForce.Get().z);
}

DirectX::SimpleMath::Vector3 PhysicsObject::CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
																      DirectX::SimpleMath::Vector3 normal)
{
	return velocity - 2.0f * (velocity * normal) * normal;
}