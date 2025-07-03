#pragma once
#include"Source/Game/Common/Collision.h"
#include"Source/Game/Common/PhysicsEngine/Gravity.h"
#include"Source/Game/Common/PhysicsEngine/ExternalForce.h"
#include"Source/Game/Common/PhysicsEngine/Fliction.h"

#include"ImaseLib/DebugFont.h"

class PhysicsObject
{
private:
	static constexpr float RESTITUTION_COEFFICIENT = 1.0f;	// 反発係数のデフォルト値

	Gravity m_gravity;
	ExternalForce m_externalForce;
	Fliction m_fliction;

public:
	//コンストラクタ・デストラクタ
	PhysicsObject();
	~PhysicsObject();

	//速度の算出
	void CalculateVelocity(DirectX::SimpleMath::Vector3& velocity,
						   float mass,
						   bool onGround,
						   float elapsedTime);

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

	//摩擦を加える
	void AddFliction(DirectX::SimpleMath::Vector3& velocity, float min = 0.1f);

	//取得
	ExternalForce& GetExternalForce() { return m_externalForce; }
	Gravity& GetGravity() { return m_gravity; }
	Fliction& GetFliction() { return m_fliction; }

	//デバッグフォントの描画
	void DrawDebugFont(Imase::DebugFont* debugFont, float y);

private:
	//反射ベクトルの算出
	DirectX::SimpleMath::Vector3 CalculateReflectionVector(DirectX::SimpleMath::Vector3 velocity,
														   DirectX::SimpleMath::Vector3 normal);
};