/**
 * @file   GroundEnemy.cpp
 *
 * @brief  地上の敵に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy.h"
#include "Source/Game/Scenes/StageScene.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param info		出現する敵の情報
 * @param pUR		ユーザーリソースのポインタ
 * @param pEM		エフェクトマネージャーのポインタ
 * @param pScene	シーンのポインタ
 */
GroundEnemy::GroundEnemy(
	const EnemyInfoLoader::EnemyInfo& info,
	UserResources* pUR, EffectManager* pEM,
	StageScene* pScene)
	: IEnemy{ info,pScene }
	, m_playerRelationData{ DirectX::SimpleMath::Vector3::Zero,0.0f }
	, m_trajectory{ nullptr }
	, m_circle{ nullptr }
{
	//m_sphere = DirectX::GeometricPrimitive::CreateSphere(pUR->GetDeviceResources()->GetD3DDeviceContext());

	// エフェクトの設定
	SetupEffects(pEM, pUR->GetResourceManager());
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
 * @param pRM		リソースマネージャーのポインタ
 * @param pCM		当たり判定マネージャーのポインタ
 * @param position	初期位置
 * @param info		出現する敵の情報
 * @param id		ID
 *
 * @return なし
 */
void GroundEnemy::Initialize(
	ResourceManager* pRM,
	CollisionManager* pCM,
	const DirectX::SimpleMath::Vector3& position,
	const EnemyInfoLoader::EnemyInfo& info,
	const bool& dropRespawn,
	uint32_t id)
{
	// 位置の初期化
	m_position = position;

	// 速度の初期化
	m_velocity = DirectX::SimpleMath::Vector3::Zero;

	// 攻撃の初期化
	m_isAttack = false;
	m_attackForce = 0.0f;

	// リスポーン位置を設定
	m_respawnPos = m_position;
	m_respawnPos.y += RESPAWN_POS_HEIGHT;

	// リスポーン有無の設定
	m_dropRespawn = dropRespawn;

	// モデル・アニメーションの設定
	SetupModels(pRM, info);

	// リソースマネージャの設定
	m_pResourceManager = pRM;

	// 物理演算の設定
	m_physics = std::make_unique<PhysicsObject>();
	m_physics->GetFriction().SetStaticFriction(STATIC_FRICTION);
	m_physics->GetFriction().SetDynamicFriction(DYNAMIC_FRICTION);

	// 当たり判定の設定
	SetupCollision(pCM, id);

	// エフェクトを出現をオフ
	m_trajectory->SetSpawn(false);

	// 状態の設定
	SetupState(pRM, info);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
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
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void GroundEnemy::Draw(RenderContext& context, DebugFont* debugFont)
{
	m_currentState->Render(context);

	// デバッグ情報の追加
	//debugFont->AddString(0, 200, DirectX::Colors::Blue, L"dist = %f", m_playerRelationData.distance);
	//debugFont->AddString(0, 230, DirectX::Colors::Blue, L"ePos  = %f,%f,%f",
	//	m_circle->position->x, m_circle->position->y, m_circle->position->z);
	//debugFont->AddString(0, 260, DirectX::Colors::Blue, L"effect  = %d", m_trajectory->spawn);
	debugFont->AddString(0, 230, DirectX::Colors::Blue, L"state= %d", GetStateType());
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void GroundEnemy::Finalize()
{
	// ステートのリセット
	// 待機状態
	if (m_idlingState) m_idlingState->Finalize();
	m_idlingState.reset();
	// 移動状態
	if (m_walkingState) m_walkingState->Finalize();
	m_walkingState.reset();
	// 攻撃状態
	if (m_attackingState) m_attackingState->Finalize();
	m_attackingState.reset();
	// 跳ね返り状態
	if (m_bouncingState) m_bouncingState->Finalize();
	m_bouncingState.reset();

	m_model = nullptr;
	m_animations.reset();

	// 当たり判定の削除
	if (m_handleBody) 
	{
		m_pCollisionManager->Remove(m_handleBody);
		m_handleBody = 0;
	}
	if (m_handleAttack)
	{
		m_pCollisionManager->Remove(m_handleAttack);
		m_handleAttack = 0;
	}
	m_pCollisionManager = nullptr;

	// エフェクトの無効化
	if (m_circle)
	{
		m_circle->effect->Deactivate();
		m_circle = nullptr;
	}
	if (m_trajectory)
	{
		m_trajectory->effect->Deactivate();
		m_trajectory = nullptr;
	}
}


/**
 * @brief リスポーン
 *
 * @param なし
 *
 * @return なし
 */
void GroundEnemy::Respawn()
{
	// 座標と速度を初期化
	m_position = m_respawnPos;
	m_velocity = DirectX::SimpleMath::Vector3::Zero;
	m_isAttack = false;
	m_collider.SetCenter(m_position);

	// エフェクトの出現
	m_circle->Spawn();

	// 跳ね返り状態に設定
	ChangeState(m_idlingState.get());
}


/**
 * @brief 状態の切り替え
 *
 * @param newState 次の状態へのポインタ
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
 * @param なし
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
 * @param pos	 プレイヤーの位置
 * @param radius プレイヤーの半径
 *
 * @return なし
 */
void GroundEnemy::CalculatePlayerRelationData(DirectX::SimpleMath::Vector3 pos, float radius)
{
	// 自身からプレイヤーへの方向を計算
	m_playerRelationData.direction = pos - m_position;

	// プレイヤーとの距離を計算
	float centerDistance = m_playerRelationData.direction.Length();
	float radiusSum = radius + RADIUS;
	m_playerRelationData.distance = std::fabs(radiusSum - centerDistance);
	m_playerRelationData.direction.Normalize();
}


/**
 * @brief 攻撃を受けて吹っ飛ぶ
 *
 * @param collider 相手のコライダー
 * @param power	   攻撃力
 *
 * @return なし
 */
void GroundEnemy::SmashPlayerAttack(SphereCollider collider, float power)
{
	MTV mtv = CalculateMTV(collider, m_collider);

	// 吹っ飛ぶ方向の設定
	DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
	knockbackDir.Normalize();

	// 外力を加える
	DirectX::SimpleMath::Vector3 force = knockbackDir * power;
	m_physics->GetExternalForce().Add(force);

	// SEの再生
	m_pScene->PlaySE("attackSE");

	// 角速度の加算
	m_physics->AddAngVelocity(DirectX::SimpleMath::Vector3(ANGULAR_VELOCITY, 0.0f, 0.0f));

	// 跳ね返り状態に遷移
	ChangeState(m_bouncingState.get());
}


/**
 * @brief 地面や壁との反射
 *
 * @param normal 法線ベクトル
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
		// 反射
		m_physics->Reflection(m_velocity, normal, RESTITUTION);
		// 円形エフェクトを発生
		m_circle->Spawn();
		break;
	}
}



/**
 * @brief エフェクトの設定
 *
 * @param pEM	エフェクトマネージャーのポインタ
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy::SetupEffects(EffectManager* pEM, ResourceManager* pRM)
{
	// エフェクトのパラメータを設定
	// 軌跡
	ParticleUtility::ParticleData t_data =
	{
		{DirectX::SimpleMath::Vector3(TRAJECTORY_SCALE),DirectX::SimpleMath::Color(1,1,1,1)},
		{DirectX::SimpleMath::Vector3(0.0f),DirectX::SimpleMath::Color(1,1,1,0)},
		TRAJECTORY_LIFE
	};
	// 円形
	ParticleUtility::ParticleData c_data =
	{
		{DirectX::SimpleMath::Vector3(CIRCLE_SCALE),DirectX::SimpleMath::Color(1,1,1,1)},
		{DirectX::SimpleMath::Vector3(0.0f),DirectX::SimpleMath::Color(1,1,1,0)},
		CIRCLE_LIFE
	};

	// エフェクトの作成
	// 軌跡
	m_trajectory = pEM->CreateTrajectory(
		pRM->RequestPNG("smoke", "Effect/smoke.png"),
		t_data,
		&m_position,
		false
	);
	// 円形
	m_circle = pEM->CreateCircle(
		pRM->RequestPNG("smoke", "Effect/smoke.png"),
		c_data,
		&m_position,
		RADIUS * 1.5f,
		12,
		false,
		true
	);
}



/**
 * @brief モデル・アニメーションの設定
 *
 * @param pRM	リソースマネージャーのポインタ
 * @param info	敵情報
 *
 * @return なし
 */
void GroundEnemy::SetupModels(ResourceManager* pRM, const EnemyInfoLoader::EnemyInfo& info)
{
	// モデルの読み込み		(キーがとりあえずパスになっているので変える)
	m_model = pRM->RequestSDKMESH(info.modelPath, info.modelPath, true);

	// アニメーションの読み込み
	m_animations = std::make_unique<Animations>();
	m_animations->idle = pRM->RequestAnimation(info.idleAnimPath, info.idleAnimPath);
	m_animations->walk = pRM->RequestAnimation(info.walkAnimPath, info.walkAnimPath);
	m_animations->attack = pRM->RequestAnimation(info.attackAnimPath, info.attackAnimPath);
}



/**
 * @brief 当たり判定の設定
 *
 * @param pCM	当たり判定マネージャーのポインタ
 * @param id	個別ID
 *
 * @return なし
 */
void GroundEnemy::SetupCollision(CollisionManager* pCM, const uint32_t& id)
{
	// 当たり判定マネージャーの登録
	m_pCollisionManager = pCM;

	// 当たり判定の設定
	m_collider.SetCenter(m_position);
	m_collider.SetRadius(RADIUS);

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
		[this](uint32_t , const DirectX::SimpleMath::Vector3& n, float)
		{
			m_physics->SetGroundInfo(n);	// 接地面の法線を渡す
			ReflectOnCollision(n);			// 跳ね返り
		};
	bodyDesc.callback.onEnter =
		[this](uint32_t, uint32_t other)	// プレイヤーの攻撃で吹っ飛ぶ
		{
			auto otherDesc = m_pCollisionManager->GetDesc(other);
			if (otherDesc->layer != CollisionManager::Layer::PlayerAttack) return;

			SmashPlayerAttack(*otherDesc->sphere, *otherDesc->userData);
		};
	bodyDesc.callback.onStay =
		[this](uint32_t, uint32_t other)	// プレイヤーの攻撃で吹っ飛ぶ(連続ヒット有の場合)
		{
			auto otherDesc = m_pCollisionManager->GetDesc(other);
			if (otherDesc->layer != CollisionManager::Layer::PlayerAttack && !otherDesc->isMultiHit) return;

			SmashPlayerAttack(*otherDesc->sphere, *otherDesc->userData);
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
}



/**
 * @brief 状態の設定
 *
 * @param pRM	リソースマネージャーのポインタ
 * @param info	敵情報
 *
 * @return なし
 */
void GroundEnemy::SetupState(ResourceManager* pRM, const EnemyInfoLoader::EnemyInfo& info)
{
	// 状態の作成
	// 待機状態
	m_idlingState = std::make_unique<GroundEnemy_Idle>(this);
	m_idlingState->Initialize(pRM);
	// 歩き状態
	m_walkingState = std::make_unique<GroundEnemy_Walk>(this, info);
	m_walkingState->Initialize(pRM);
	// 跳ね返り状態
	m_bouncingState = std::make_unique<GroundEnemy_Bounce>(this);
	m_bouncingState->Initialize(pRM);
	// 攻撃状態
	m_attackingState = std::make_unique<GroundEnemy_Attack>(this, info);
	m_attackingState->Initialize(pRM);

	// 初期状態の設定
	m_currentState = m_idlingState.get();
}