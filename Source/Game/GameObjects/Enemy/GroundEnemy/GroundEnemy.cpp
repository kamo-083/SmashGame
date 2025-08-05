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
GroundEnemy::GroundEnemy(UserResources* pUserResources)
	:Enemy{}
	,m_currentState{nullptr}
	,m_playerRelationData{DirectX::SimpleMath::Vector3::Zero,0.0f}
{
	m_sphere = DirectX::GeometricPrimitive::CreateSphere(pUserResources->GetDeviceResources()->GetD3DDeviceContext());

	// 軌跡エフェクトの作成
	m_trajectory = std::make_unique<TrajectoryParticle>();
	m_trajectory->Create(
		pUserResources->GetDeviceResources(),
		L"Resources/Textures/Effect/smoke.png",
		0.5f,
		2.0f,
		SimpleMath::Color(1, 1, 1, 1));
}


/**
 * @brief デストラクタ
 */
GroundEnemy::~GroundEnemy()
{

}


void GroundEnemy::Initialize(ResourceManager* pResourceManager)
{
	// 座標の初期化
	m_position = SimpleMath::Vector3(0.0f, 5.0f, -4.0f);

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
	m_physics.GetFriction().SetStaticFriction(0.5f);
	m_physics.GetFriction().SetDynamicFriction(0.5f);

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
	
	m_trajectory->Render(context.view, context.projection);

	debugFont->AddString(0, 200, DirectX::Colors::Blue, L"dist = %f", m_playerRelationData.distance);
	debugFont->AddString(0, 230, DirectX::Colors::Blue, L"vel  = %f,%f,%f",
		m_velocity.x, m_velocity.y, m_velocity.z);
	debugFont->AddString(0, 260, DirectX::Colors::Blue, L"atk  = %d", m_isAttack);
	debugFont->AddString(0, 290, DirectX::Colors::Blue, L"state= %d", GetStateType());
}


void GroundEnemy::Finalize()
{
	m_idlingState.reset();
}


void GroundEnemy::ChangeState(IState* newState)
{
	// 新規の状態を現在の状態に設定する
	m_currentState = newState;

	// 状態を初期化
	m_currentState->Initialize(m_pResourceManager);
}


void GroundEnemy::UpdateEffect(float elapsedTime, Camera* camera)
{
	// 軌跡エフェクトの更新
	m_trajectory->Update(elapsedTime, m_position, GetStateType() == StateType::Bounce);
	m_trajectory->CreateBillboard(m_position, camera->GetTarget(), camera->GetEye(), camera->GetUp());
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
				m_physics.Reflection(m_velocity, m_surfaceNormal, 0.8f);
			}
			else
			{
				m_onGround = true;
				m_velocity.y = 0.0f;
				m_surfaceNormal = normal;
			}
			break;
		case OBBCollider::CollisionType::Wall:
			m_physics.Reflection(m_velocity, normal, 0.8);
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

		m_physics.Reflection(m_velocity, normal, 0.8);
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
		m_physics.GetExternalForce().Add(force);

		// 跳ね返り状態に遷移
		ChangeState(m_bouncingState.get());
	}

	return hit;
}

