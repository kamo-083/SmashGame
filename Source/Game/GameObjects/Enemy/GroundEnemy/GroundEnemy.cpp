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

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] info				敵の情報
 * @param[in] pUserResources	ユーザーリソースのポインタ
 * @param[in] pEffectManager エフェクトマネージャーのポインタ
 */
GroundEnemy::GroundEnemy(const EnemyInfoLoader::EnemyInfo& info, UserResources* pUserResources, EffectManager* pEffectManager)
	: Enemy{ info }
	, m_playerRelationData{ DirectX::SimpleMath::Vector3::Zero,0.0f }
	, m_trajectory{ nullptr }
	, m_circle{ nullptr }
{
	//m_sphere = DirectX::GeometricPrimitive::CreateSphere(pUserResources->GetDeviceResources()->GetD3DDeviceContext());

	// 軌跡エフェクトの作成
	m_trajectory = pEffectManager->CreateTrajectory(
		pUserResources->GetResourceManager()->RequestPNG("smoke", L"Resources/Textures/Effect/smoke.png"),
		0.5f,
		2.0f,
		SimpleMath::Color(1, 1, 1, 1),
		&m_position,
		false
	);

	// 円形エフェクトの作成
	m_circle = pEffectManager->CreateCircle(
		pUserResources->GetResourceManager()->RequestPNG("smoke", L"Resources/Textures/Effect/smoke.png"),
		0.75f,
		1.0f,
		SimpleMath::Color(1, 1, 1, 1),
		&m_position,
		RADIUS * 1.5f,
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


/**
 * @brief 初期化処理
 *
 * @param[in] pResourceManager  リソースマネージャーのポインタ
 * @param[in] pCollisionManager コリジョンマネージャーのポインタ
 * @param[in] position			初期位置
 * @param[in] id				ID
 *
 * @return なし
 */
void GroundEnemy::Initialize(ResourceManager* pResourceManager,
							 CollisionManager* pCollisionManager,
							 const DirectX::SimpleMath::Vector3& position,
							 const EnemyInfoLoader::EnemyInfo& info,
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

	// モデルの読み込み		(キーがとりあえずパスになっているので変える)
	m_model = pResourceManager->RequestSDKMESH(info.modelPath, info.modelPath, true);

	// アニメーションの読み込み
	m_animations = std::make_unique<Animations>();
	m_animations->idle = pResourceManager->RequestAnimation(info.idleAnimPath, info.idleAnimPath);
	m_animations->walk = pResourceManager->RequestAnimation(info.walkAnimPath, info.walkAnimPath);
	m_animations->attack = pResourceManager->RequestAnimation(info.attackAnimPath, info.attackAnimPath);

	// リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	// 当たり判定の設定
	m_collider.SetCenter(m_position);
	m_collider.SetRadius(RADIUS);

	// 物理演算の設定
	m_physics = std::make_unique<PhysicsObject>();
	m_physics->GetFriction().SetStaticFriction(STATIC_FRICTION);
	m_physics->GetFriction().SetDynamicFriction(DYNAMIC_FRICTION);

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
	bodyDesc.mass = MASS;
	bodyDesc.callback.onResolved =
		[this](uint32_t other, const SimpleMath::Vector3& n, float)	// 接地フラグを立てる
		{
			const float groundCos = std::cos(XMConvertToRadians(30.0f));
			if (n.y >= groundCos) m_onGround = true;

			// 地面・壁との反射
			if (m_pCollisionManager->GetDesc(other)->layer == CollisionManager::Layer::Stage) ReflectOnCollision(n);
		};
	bodyDesc.callback.onEnter =
		[this](uint32_t, uint32_t other)	// プレイヤーの攻撃で吹っ飛ぶ
		{
			auto otherDesc = m_pCollisionManager->GetDesc(other);
			if (otherDesc->layer != CollisionManager::Layer::PlayerAttack) return;

			DetectCollisionToAttack(*otherDesc->sphere, *otherDesc->userData);
		};
	bodyDesc.callback.onStay =
		[this](uint32_t, uint32_t other)	// プレイヤーの攻撃で吹っ飛ぶ(連続ヒット有の場合)
		{			
			auto otherDesc = m_pCollisionManager->GetDesc(other);
			if (otherDesc->layer != CollisionManager::Layer::PlayerAttack && !otherDesc->isMultiHit) return;

			DetectCollisionToAttack(*otherDesc->sphere, *otherDesc->userData);
		};
	m_handleBody = m_pCollisionManager->Add(bodyDesc);
	// 攻撃
	CollisionManager::Desc atkDesc{};
	atkDesc.type = CollisionManager::Type::Sphere;
	atkDesc.layer = CollisionManager::Layer::EnemyAttack;
	atkDesc.isTrigger = true;
	atkDesc.sphere = &m_attackCollider;
	atkDesc.userData = &m_attackForce;
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
	m_attackingState = std::make_unique<GroundEnemy_Attack>(this, info);
	m_attackingState->Initialize(pResourceManager);

	// エフェクトを出現をオフ
	m_trajectory->SetSpawn(false);

	// 初期状態の設定
	m_currentState = m_idlingState.get();
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy::Update(float elapsedTime)
{
	m_currentState->Update(elapsedTime);
}


/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 * @param[in] debugFont デバッグ用フォント
 *
 * @return なし
 */
void GroundEnemy::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	m_currentState->Render(context);
	
	debugFont->AddString(0, 200, DirectX::Colors::Blue, L"dist = %f", m_playerRelationData.distance);
	//debugFont->AddString(0, 230, DirectX::Colors::Blue, L"ePos  = %f,%f,%f",
	//	m_circle->position->x, m_circle->position->y, m_circle->position->z);
	//debugFont->AddString(0, 260, DirectX::Colors::Blue, L"effect  = %d", m_trajectory->spawn);
	debugFont->AddString(0, 230, DirectX::Colors::Blue, L"state= %d", GetStateType());
}


/**
 * @brief 終了処理
 *
 * @param[in] pCollisionManager コリジョンマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy::Finalize(CollisionManager* pCollisionManager)
{
	m_idlingState.reset();

	m_model = nullptr;
	m_animations.reset();

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

	m_circle = nullptr;

	if (m_trajectory)
	{
		m_trajectory->effect->Deactivate();
		m_trajectory = nullptr;
	}
}


/**
 * @brief 状態の切り替え
 *
 * @param[in] newState 次の状態へのポインタ
 *
 * @return なし
 */
void GroundEnemy::ChangeState(IState* newState)
{
	// 新規の状態を現在の状態に設定する
	m_currentState = newState;

	// 状態を初期化
	m_currentState->Initialize(m_pResourceManager);
}


/**
 * @brief 移動速度の制限
 *
 * @param[in] なし
 *
 * @return なし
 */
void GroundEnemy::LimitVelocity()
{
	m_velocity.x = std::min(std::max(m_velocity.x, -MAX_SPEED), MAX_SPEED);
	m_velocity.y = std::min(std::max(m_velocity.y, -MAX_SPEED), MAX_SPEED);
	m_velocity.z = std::min(std::max(m_velocity.z, -MAX_SPEED), MAX_SPEED);
}


/**
 * @brief プレイヤーとの位置関係を計算
 *
 * @param[in] pos	 プレイヤーの位置
 * @param[in] radius プレイヤーの半径
 *
 * @return なし
 */
void GroundEnemy::CalculatePlayerRelationData(DirectX::SimpleMath::Vector3 pos, float radius)
{
	m_playerRelationData.direction = pos - m_position;

	float centerDistance = m_playerRelationData.direction.Length();
	float radiusSum = radius + RADIUS;
	m_playerRelationData.distance = std::fabs(radiusSum - centerDistance);

	m_playerRelationData.direction.Normalize();
}


/**
 * @brief 攻撃を受けた時の処理
 *
 * @param[in] collider 相手のコライダー
 * @param[in] power	   攻撃力
 *
 * @return 攻撃が
 */
void GroundEnemy::DetectCollisionToAttack(SphereCollider collider, float power)
{
	MTV mtv = CalculateMTV(collider, m_collider);

	// 吹っ飛ぶ方向の設定
	DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
	knockbackDir.Normalize();
	
	DirectX::SimpleMath::Vector3 force = knockbackDir * power;
	m_physics->GetExternalForce().Add(force);

	// 跳ね返り状態に遷移
	ChangeState(m_bouncingState.get());
}


/**
 * @brief 地面や壁との反射
 *
 * @param[in] normal 法線ベクトル
 *
 * @return なし
 */
void GroundEnemy::ReflectOnCollision(DirectX::SimpleMath::Vector3 normal)
{
	if (m_currentState != m_bouncingState.get()) return;

	switch (DetermineCollisionType(normal))
	{
	case OBBCollider::CollisionType::Ground:
		// 円形エフェクトを発生
		m_circle->Spawn();
		break;
	case OBBCollider::CollisionType::Wall:
		m_physics->Reflection(m_velocity, normal, 0.8);

		// 円形エフェクトを発生
		m_circle->Spawn();
		break;
	}
}

