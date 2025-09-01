/**
 * @file   GroundEnemy.cpp
 *
 * @brief  敵に関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy.h"

using namespace DirectX;

// 定数
const float Enemy::RADIUS = 0.5f;		// 半径
const float Enemy::SPEED = 5.0f;		// 移動速度
const float Enemy::MASS = 5.0f;			// 質量
const float Enemy::MAX_SPEED = 10.0f;	// 最高速度


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GroundEnemy::GroundEnemy(UserResources* pUserResources,
						 EffectManager* pEffectManager)
	:Enemy{}
	,m_playerRelationData{DirectX::SimpleMath::Vector3::Zero,0.0f}
{
	m_sphere = DirectX::GeometricPrimitive::CreateSphere(pUserResources->GetDeviceResources()->GetD3DDeviceContext());

	// 軌跡エフェクトの作成
	m_trajectory = pEffectManager->CreateTrajectory(
		pUserResources->GetResourceManager()->RequestTexture("smoke", L"Resources/Textures/Effect/smoke.png"),
		0.5f,
		2.0f,
		SimpleMath::Color(1, 1, 1, 1),
		&m_position,
		false
	);

	// 円形エフェクトの作成
	m_circle = pEffectManager->CreateCircle(
		pUserResources->GetResourceManager()->RequestTexture("smoke", L"Resources/Textures/Effect/smoke.png"),
		0.75f,
		1.0f,
		SimpleMath::Color(1, 1, 1, 1), 
		&m_position,
		1.0f, 
		12,
		false,
		true
	);

}


/**
 * @brief デストラクタ
 */
GroundEnemy::~GroundEnemy()
{
	m_sphere.reset();
}


void GroundEnemy::Initialize(ResourceManager* pResourceManager,
							 CollisionManager* pCollisionManager,
							 const DirectX::SimpleMath::Vector3& position,
							 uint32_t id)
{ 
	// 座標の初期化
	m_position = SimpleMath::Vector3(position);

	// 速度の初期化
	m_velocity = SimpleMath::Vector3::Zero;

	// 着地判定の初期化
	m_onGround = false;

	// 攻撃の初期化
	m_isAttack = false;
	m_attackForce = 0.0f;

	// 法線ベクトルの初期化
	m_surfaceNormal = SimpleMath::Vector3::Zero;

	// モデルの読み込み
	m_model = pResourceManager->RequestSDKMESH("enemy", L"Resources\\Models\\cat.sdkmesh");

	// リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	// 当たり判定の設定
	m_collider.SetCenter(m_position);
	m_collider.SetRadius(RADIUS);

	// 物理演算の設定
	m_physics = std::make_unique<PhysicsObject>();
	m_physics->GetFriction().SetStaticFriction(0.5f);
	m_physics->GetFriction().SetDynamicFriction(0.15f);	// 元は0.5f

	// コリジョンマネージャーの登録
	m_pCollisionManager = pCollisionManager;

	// 本体
	CollisionManager::Desc bodyDesc{};
	bodyDesc.type = CollisionManager::Type::Sphere;
	bodyDesc.layer = CollisionManager::Layer::EnemyBody;
	bodyDesc.userId = id;
	bodyDesc.sphere = &m_collider;
	bodyDesc.position = &m_position;
	bodyDesc.velocity = &m_velocity;
	bodyDesc.callback.onResolved =
		[this](uint32_t, const SimpleMath::Vector3& n, float)	// 接地フラグを立てる
		{
			const float groundCos = std::cos(XMConvertToRadians(30.0f));
			if (n.y >= groundCos) m_onGround = true;
		};
	bodyDesc.callback.onEnter =
		[this](uint32_t, uint32_t other)		// プレイヤーの攻撃で吹っ飛ぶ
		{			
			if (m_pCollisionManager->GetDesc(other)->layer != CollisionManager::Layer::PlayerAttack) return;

			MTV mtv = CalculateMTV(*m_pCollisionManager->GetDesc(other)->sphere, m_collider);

			// 吹っ飛ぶ方向の設定
			DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
			knockbackDir.Normalize();

			// 吹っ飛ぶ力の設定
			float knockbackForce = *m_pCollisionManager->GetDesc(other)->uerData;

			DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
			m_physics->GetExternalForce().Add(force);

			// 跳ね返り状態に遷移
			ChangeState(m_bouncingState.get());
		};
	m_handleBody = m_pCollisionManager->Add(bodyDesc);
	// 攻撃
	CollisionManager::Desc atkDesc{};
	atkDesc.type = CollisionManager::Type::Sphere;
	atkDesc.layer = CollisionManager::Layer::EnemyAttack;
	atkDesc.isTrigger = true;
	atkDesc.sphere = &m_attackCollider;
	atkDesc.uerData = &m_attackForce;
	m_handleAttack = m_pCollisionManager->Add(atkDesc);

	// 状態の作成
	// 待機状態
	m_idlingState = std::make_unique<GroundEnemy_Idle>(this);
	m_idlingState->Initialize(pResourceManager);
	// 歩き状態
	m_walkingState = std::make_unique<GroundEnemy_Walk>(this);
	m_walkingState->Initialize(pResourceManager);
	// 跳ね返り状態
	m_bouncingState = std::make_unique<GroundEnemy_Bounce>(this);
	m_bouncingState->Initialize(pResourceManager);
	// 攻撃状態
	m_attackingState = std::make_unique<GroundEnemy_Attack>(this);
	m_attackingState->Initialize(pResourceManager);

	// エフェクトを出現をオフ
	m_trajectory->SetSpawn(false);

	// 初期状態の設定
	m_currentState = m_idlingState.get();
}


void GroundEnemy::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);
}


void GroundEnemy::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	m_currentState->Render(context);
	
	debugFont->AddString(0, 200, DirectX::Colors::Blue, L"dist = %f", m_playerRelationData.distance);
	//debugFont->AddString(0, 230, DirectX::Colors::Blue, L"ePos  = %f,%f,%f",
	//	m_circle->position->x, m_circle->position->y, m_circle->position->z);
	//debugFont->AddString(0, 260, DirectX::Colors::Blue, L"effect  = %d", m_trajectory->spawn);
	debugFont->AddString(0, 230, DirectX::Colors::Blue, L"state= %d", GetStateType());
}


void GroundEnemy::Finalize(CollisionManager* pCollisionManager)
{
	m_idlingState.reset();

	m_model = nullptr;

	if (m_handleBody) 
	{
		pCollisionManager->Remove(m_handleBody);
		m_handleBody = 0;
	}
	if (m_handleAttack) 
	{
		pCollisionManager->Remove(m_handleAttack);
		m_handleAttack = 0;
	}
}


void GroundEnemy::ChangeState(IState* newState)
{
	// 新規の状態を現在の状態に設定する
	m_currentState = newState;

	// 状態を初期化
	m_currentState->Initialize(m_pResourceManager);
}


void GroundEnemy::LimitVelocity()
{
	m_velocity.x = std::min(std::max(m_velocity.x, -MAX_SPEED), MAX_SPEED);
	m_velocity.y = std::min(std::max(m_velocity.y, -MAX_SPEED), MAX_SPEED);
	m_velocity.z = std::min(std::max(m_velocity.z, -MAX_SPEED), MAX_SPEED);
}


void GroundEnemy::CalculatePlayerRelationData(DirectX::SimpleMath::Vector3 pos, float radius)
{
	m_playerRelationData.direction = pos - m_position;

	float centerDistance = m_playerRelationData.direction.Length();
	float radiusSum = radius + RADIUS;
	m_playerRelationData.distance = std::fabs(radiusSum - centerDistance);

	m_playerRelationData.direction.Normalize();
}


bool GroundEnemy::DetectCollisionToBox(OBBCollider collider)
{
	bool hit = IsHit(collider, m_collider);

	if (hit)
	{
		MTV mtv = CalculateMTV(collider, m_collider);
		if (mtv.distance > 0.0f && mtv.direction.LengthSquared() > 0.00001f)
		{
			m_position += mtv.direction * mtv.distance;
			m_collider.SetCenter(m_position);
		}

		//法線ベクトルを元にオブジェクトの種類を判定
		DirectX::SimpleMath::Vector3 normal = mtv.direction;
		normal.Normalize();

		m_collisionType = DetermineCollisionType(normal);
		switch (m_collisionType)
		{
		case OBBCollider::CollisionType::Ground:
			if (m_currentState==m_bouncingState.get())
			{
				m_physics->Reflection(m_velocity, m_surfaceNormal, 0.8f);

				// 円形エフェクトを発生
				m_circle->Spawn();
			}
			else
			{
				m_onGround = true;
				m_velocity.y = 0.0f;
				m_surfaceNormal = normal;
			}
			break;
		case OBBCollider::CollisionType::Wall:
			m_physics->Reflection(m_velocity, normal, 0.8);

			// 円形エフェクトを発生
			m_circle->Spawn();
			break;
		case OBBCollider::CollisionType::Slope:
			m_onGround = true;
			m_surfaceNormal = normal;
			break;
		default:
			break;
		}
	}

	return hit;
}


bool GroundEnemy::DetectCollisionToSphere(SphereCollider collider)
{
	bool hit = IsHit(collider, m_collider);

	if (hit)
	{
		MTV mtv = CalculateMTV(collider, m_collider);
		if (mtv.distance > 0.0f && mtv.direction.LengthSquared() > 0.00001f)
		{
			m_position += mtv.direction * mtv.distance;
			m_collider.SetCenter(m_position);
		}

		//法線ベクトルを元にオブジェクトの種類を判定
		DirectX::SimpleMath::Vector3 normal = mtv.direction;
		normal.Normalize();

		m_physics->Reflection(m_velocity, normal, 0.8);
	}

	return hit;
}


bool GroundEnemy::DetectCollisionToAttack(SphereCollider collider, float power)
{
	bool hit = IsHit(collider, m_collider);

	if (hit)
	{
		MTV mtv = CalculateMTV(collider, m_collider);

		// 吹っ飛ぶ方向の設定
		DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
		knockbackDir.Normalize();

		// 吹っ飛ぶ力の設定
		float knockbackForce = mtv.distance * power;

		DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
		m_physics->GetExternalForce().Add(force);

		// 跳ね返り状態に遷移
		ChangeState(m_bouncingState.get());
	}

	return hit;
}

