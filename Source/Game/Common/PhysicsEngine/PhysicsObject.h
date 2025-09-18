#pragma once
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/PhysicsEngine/Gravity.h"
#include"Source/Game/Common/PhysicsEngine/ExternalForce.h"
#include"Source/Game/Common/PhysicsEngine/Friction.h"

#include"ImaseLib/DebugFont.h"

class PhysicsObject
{
private:
	static constexpr float RESTITUTION_COEFFICIENT = 1.0f;	// 反発係数のデフォルト値

	Gravity m_gravity;
	ExternalForce m_externalForce;
	Friction m_friction;

public:
	//コンストラクタ・デストラクタ
	PhysicsObject();
	~PhysicsObject();

	//速度の算出
	void CalculateForce(DirectX::SimpleMath::Vector3& velocity,
						float mass,
						float elapsedTime,
						bool onGround);

	//跳ね返り
	void Reflection(DirectX::SimpleMath::Vector3& velocity,
					DirectX::SimpleMath::Vector3& normal,
					float restitution = RESTITUTION_COEFFICIENT);

	//斜面の転がり
	void RollDown(DirectX::SimpleMath::Vector3& velocity,
				  DirectX::SimpleMath::Vector3& normal, 
				  float mass,
				  float radius,
				  float elapsedTime);

	//取得
	ExternalForce& GetExternalForce() { return m_externalForce; }
	Gravity& GetGravity() { return m_gravity; }
	Friction& GetFriction() { return m_friction; }

	//デバッグフォントの描画
	void DrawDebugFont(Imase::DebugFont* debugFont, float y);

private:
	//反射ベクトルの算出
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
														   DirectX::SimpleMath::Vector3 normal);
};