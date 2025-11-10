/**
 * @file   Player.cpp
 *
 * @brief  プレイヤーに関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player.h"
#include "Source/Game/Scenes/StageScene.h"
#include "Source/Game/UI/Controls/AttackUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR		ユーザーリソースのポインタ
 * @param pEM		エフェクトマネージャーのポインタ
 * @param pScene	シーンへのポインタ
 * @param info		外部ファイルからの情報
 */
Player::Player(
	UserResources* pUR, EffectManager* pEM,
	StageScene* pScene, const PlayerInfoLoader::PlayerInfo& info)
	: RADIUS{ info.radius }
	, MASS{ info.mass }
	, MAX_SPEED{ info.max_speed }
	, DYNAMIC_FRICTION_FORCE{ info.dynamic_friction }
	, STATIC_FRICTION_FORCE{ info.static_friction }
	, m_pScene{ pScene }
	, m_rotY{ 0.0f }
	, m_onGround{ false }
	, m_isBounce{ false }
	, m_model{ nullptr }
	, m_pResourceManager{ nullptr }
	, m_collider{}
	, m_attackForce{ 0.0f }
	, m_isAttack{ false }
	, m_attackCollider{}
	, m_attackType{}
	, m_pAttackUI{ nullptr }
	, m_currentState{ nullptr }
	, m_pKeyMode{ nullptr }
	, m_pCollisionManager{ nullptr }
	, m_handleBody{ 0 }
	, m_handleAttack{ 0 }
{
	// 当たり判定のデバッグ描画用球
	//m_sphere = DirectX::GeometricPrimitive::CreateSphere(pUR->GetDeviceResources()->GetD3DDeviceContext());

	// エフェクトの設定
	SetupEffects(pEM, pUR->GetResourceManager());
}



/**
 * @brief デストラクタ
 */
Player::~Player()
{
	m_sphere.reset();
}



/**
 * @brief 初期化処理
 *
 * @param param 引数群
 * 
 * pRM			リソースマネージャーのポインタ
 * pCM			当たり判定マネージャーのポインタ
 * pKbTracker	キーボードトラッカーのポインタ
 * pCamera		カメラのポインタ
 * pAttackUI	攻撃UIのポインタ
 * pKeyMode		キー入力モードのポインタ
 * info			外部ファイルからの情報
 *
 * @return なし
 */
void Player::Initialize(PlayerParams param)
{
	// 位置の初期化
	m_position = START_POS;

	// 速度の初期化
	m_velocity = DirectX::SimpleMath::Vector3::Zero;

	// 向きの初期化
	m_rotY = 0.0f;

	// 着地
	m_onGround = false;

	// 吹っ飛ばされ状態
	m_isBounce = false;

	// 攻撃の設定
	m_attackType = AttackType::BASIC;

	// 攻撃UIの設定
	m_pAttackUI = param.pAttackUI;

	// リソースマネージャの設定
	m_pResourceManager = param.pRM;

	// モデル・アニメーションの取得
	SetupModels(param.pRM, param.info);

	// キー操作のモードのポインタの設定
	m_pKeyMode = param.pKeyMode;

	// 物理演算の設定
	m_physics = std::make_unique<PhysicsObject>();
	m_physics->GetFriction().SetDynamicFriction(DYNAMIC_FRICTION_FORCE);
	m_physics->GetFriction().SetStaticFriction(STATIC_FRICTION_FORCE);

	// 当たり判定の設定
	SetupCollision(param.pCM);

	// 状態の設定
	SetupState(param.pKbTracker, param.pCamera, param.info);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Player::Update(const float& elapsedTime)
{
	// 現在の状態を更新する
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
void Player::Draw(RenderContext& context, DebugFont* debugFont)
{
	// 現在の状態を描画する
	m_currentState->Render(context);

	// デバッグ情報の追加
	debugFont->AddString(0, 60, DirectX::Colors::Cyan, L"pos = %f,%f,%f", m_position.x, m_position.y, m_position.z);
	debugFont->AddString(0, 85, DirectX::Colors::Cyan, L"vel = %f,%f,%f", m_velocity.x, m_velocity.y, m_velocity.z);
	debugFont->AddString(0, 110, DirectX::Colors::Cyan, L"attack = %d", static_cast<int>(m_attackType));
	debugFont->AddString(140, 110, DirectX::Colors::Cyan, L"bounce = %d", static_cast<int>(m_isBounce));
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Player::Finalize()
{
	// 状態をリセットする
	// 待機状態
	if (m_idlingState) m_idlingState->Finalize();
	m_idlingState.reset();
	// 移動状態
	if (m_walkingState) m_walkingState->Finalize();
	m_walkingState.reset();
	// 通常攻撃状態
	if (m_basicAttackingState) m_basicAttackingState->Finalize();
	m_basicAttackingState.reset();
	// 転がり攻撃状態
	if (m_rollingAttackingState) m_rollingAttackingState->Finalize();
	m_rollingAttackingState.reset();
	// 強攻撃状態
	if (m_heavyAttackingState) m_heavyAttackingState->Finalize();
	m_heavyAttackingState.reset();

	m_model = nullptr;

	// エフェクトの後処理
	m_circle->effect->Deactivate();
	m_circle = nullptr;
	m_trajectory->effect->Deactivate();
	m_trajectory = nullptr;

	// 当たり判定の後処理
	m_pCollisionManager->Remove(m_handleBody);
	m_pCollisionManager->Remove(m_handleAttack);
	m_pCollisionManager = nullptr;
}



/**
 * @brief 状態の切り替え
 *
 * @param newState 次の状態へのポインタ
 *
 * @return なし
 */
void Player::ChangeState(IState* newState)
{
	// 新規の状態を現在の状態に設定する
	m_currentState = newState;

	// 状態を初期化
	m_currentState->Initialize(m_pResourceManager);
}



/**
 * @brief 攻撃の切り替え
 *
 * @param pKbTracker キーボードトラッカーのポインタ
 *
 * @return なし
 */
void Player::ChangeAttack(DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	if (!(*m_pKeyMode)) return;

	if (pKbTracker->pressed.C)
	{
		++m_attackType;

		// SEの再生
		m_pScene->PlaySE("cursorSE");
	}
	else if (pKbTracker->pressed.Z)
	{
		--m_attackType;

		// SEの再生
		m_pScene->PlaySE("cursorSE");
	}

	m_pAttackUI->ChangeAttack(m_attackType);
}



/**
 * @brief 攻撃処理
 *
 * @param なし
 *
 * @return なし
 */
void Player::Attack()
{
	m_isAttack = true;
	SetAttackCollisionEnabled(true);

	switch (m_attackType)
	{
	case AttackType::BASIC:
		// 攻撃判定の連続ヒットの無効化
		SetAttackCollisionMultiHit(false);
		// 状態の変更
		ChangeState(m_basicAttackingState.get());
		break;
	case AttackType::ROLLING:
		// 攻撃判定の連続ヒットの有効化
		SetAttackCollisionMultiHit(true);
		// 状態の変更
		ChangeState(m_rollingAttackingState.get());
		break;
	case AttackType::HEAVY:
		// 攻撃判定の連続ヒットの無効化
		SetAttackCollisionMultiHit(false);
		// 状態の変更
		ChangeState(m_heavyAttackingState.get());
		break;
	default:
		break;
	}
}


/**
 * @brief リスポーン
 *
 * @param なし
 *
 * @return なし
 */
void Player::Respawn()
{
	// 位置の初期化
	m_position = START_POS;

	// 速度の初期化
	m_velocity = DirectX::SimpleMath::Vector3::Zero;

	// 向きの初期化
	m_rotY = 0.0f;

	// 着地
	m_onGround = false;

	// 吹っ飛ばされ状態
	m_isBounce = false;

	// 軌跡エフェクトをオフ
	m_trajectory->SetSpawn(false);

	// 状態の切り替え
	ChangeState(m_idlingState.get());
}



/**
 * @brief 移動方向を計算
 *
 * @param kbTracker キーボードトラッカーのポインタ
 * @param camera	カメラのポインタ
 *
 * @return 移動方向
 */
DirectX::SimpleMath::Vector3 Player::MoveDirection(
	DirectX::Keyboard::KeyboardStateTracker* kbTracker,
	Camera* camera)
{
	DirectX::SimpleMath::Vector3 forward = camera->GetForward();
	DirectX::SimpleMath::Vector3 right = forward.Cross(camera->GetUp());
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::Zero;

	//移動
	if (kbTracker->GetLastState().Down)	// 後ろ
	{
		direction += -forward;
	}
	else if (kbTracker->GetLastState().Up)	// 前
	{
		direction -= -forward;
	}
	if (kbTracker->GetLastState().Right)	// 右
	{
		direction += right;
	}
	else if (kbTracker->GetLastState().Left)	// 左
	{
		direction -= right;
	}

	direction.y = 0.0f;
	if (direction.Length() > 0.0f)
	{
		direction.Normalize();
	}

	//回転
	if (direction.x != 0.0f || direction.z != 0.0f)
	{
		m_rotY = std::atan2f(-direction.x, -direction.z);
	}

	return direction;
}



/**
 * @brief 移動速度の制限
 *
 * @param velocity	現在の移動速度
 * @param max		速度上限
 *
 * @return なし
 */
void Player::LimitVelocity(DirectX::SimpleMath::Vector3& velocity, float max)
{
	velocity.x = std::min(std::max(velocity.x, -max), max);
	velocity.y = std::min(std::max(velocity.y, -MAX_SPEED), MAX_SPEED);
	velocity.z = std::min(std::max(velocity.z, -max), max);
}



/**
 * @brief 攻撃判定の有効/無効化
 *
 * @param enabled 有効/無効
 *
 * @return なし
 */
void Player::SetAttackCollisionEnabled(bool enabled)
{
	m_pCollisionManager->SetEnabled(m_handleAttack, enabled);
}



/**
 * @brief 攻撃判定の有効/無効を取得
 *
 * @param なし
 *
 * @return 有効/無効
 */
bool Player::GetAttackCollisionEnabled()
{
	return m_pCollisionManager->IsEnabled(m_handleAttack);
}



/**
 * @brief 攻撃判定の連続ヒットの有効/無効化
 *
 * @param multiHit 有効/無効
 *
 * @return なし
 */
void Player::SetAttackCollisionMultiHit(bool multiHit)
{
	m_pCollisionManager->SetMultiHit(m_handleAttack, multiHit);
}



/**
 * @brief 移動キーが押されているか
 *
 * @param pKbTracker キーボードトラッカーのポインタ
 *
 * @return なし
 */
bool Player::PressMoveKey(DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	return pKbTracker->GetLastState().Up   || 
		   pKbTracker->GetLastState().Down || 
		   pKbTracker->GetLastState().Left ||
		   pKbTracker->GetLastState().Right;
}



/**
 * @brief 敵の攻撃で吹っ飛ぶ
 *
 * @param handle	攻撃判定のハンドル
 *
 * @return なし
 */
void Player::SmashEnemyAttack(const uint32_t& handle)
{
	MTV mtv = CalculateMTV(*m_pCollisionManager->GetDesc(handle)->sphere, m_collider);

	// 吹っ飛ぶ方向の設定
	DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
	knockbackDir.Normalize();

	// 吹っ飛ぶ力の設定
	float knockbackForce = *m_pCollisionManager->GetDesc(handle)->userData;

	DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
	m_physics->GetExternalForce().Add(force);

	// SEの再生
	m_pScene->PlaySE("attackSE");

	// 跳ね返り状態に遷移
	m_isBounce = true;
	m_trajectory->SetSpawn(true);
	ChangeState(m_idlingState.get());
}



/**
 * @brief エフェクトの設定
 *
 * @param pEM	エフェクトマネージャーのポインタ
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void Player::SetupEffects(EffectManager* pEM, ResourceManager* pRM)
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
 * @brief 当たり判定の設定
 *
 * @param pCM	当たり判定マネージャーのポインタ
 *
 * @return なし
 */
void Player::SetupCollision(CollisionManager* pCM)
{
	// 当たり判定マネージャーの登録
	m_pCollisionManager = pCM;

	// コライダーの設定
	m_collider = SphereCollider(m_position, RADIUS);

	// 攻撃判定の設定
	m_attackCollider = SphereCollider(m_position, RADIUS);

	// 本体
	CollisionManager::Desc bodyDesc{};
	bodyDesc.type = CollisionManager::Type::Sphere;
	bodyDesc.layer = CollisionManager::Layer::PlayerBody;
	bodyDesc.sphere = &m_collider;
	bodyDesc.position = &m_position;
	bodyDesc.velocity = &m_velocity;
	bodyDesc.mass = MASS;
	bodyDesc.callback.onResolved =
		[this](uint32_t, const DirectX::SimpleMath::Vector3& n, float)	// 接地フラグを立てる
		{
			const float groundCos = std::cos(DirectX::XMConvertToRadians(30.0f));
			if (n.y >= groundCos) m_onGround = true;
		};
	bodyDesc.callback.onEnter =
		[this](uint32_t, uint32_t other)		// 敵の攻撃で吹っ飛ぶ
		{
			if (m_pCollisionManager->GetDesc(other)->layer != CollisionManager::Layer::EnemyAttack) return;

			SmashEnemyAttack(other);
		};
	m_handleBody = m_pCollisionManager->Add(bodyDesc);
	// 攻撃
	CollisionManager::Desc atkDesc{};
	atkDesc.type = CollisionManager::Type::Sphere;
	atkDesc.layer = CollisionManager::Layer::PlayerAttack;
	atkDesc.isTrigger = true;
	atkDesc.sphere = &m_attackCollider;
	atkDesc.userData = &m_attackForce;
	m_handleAttack = m_pCollisionManager->Add(atkDesc);
	m_pCollisionManager->SetEnabled(m_handleAttack, false);
}



/**
 * @brief 状態の設定
 *
 * @param pKeyboard	キーボードトラッカーのポインタ
 * @param pCamera	カメラのポインタ
 * @param info		外部ファイルからの情報
 *
 * @return なし
 */
void Player::SetupState(
	DirectX::Keyboard::KeyboardStateTracker* pKeyboard, Camera* pCamera,
	const PlayerInfoLoader::PlayerInfo& info)
{
	// 待機状態を生成
	m_idlingState = std::make_unique<Player_Idle>(this, pKeyboard);

	// 歩き状態を生成
	m_walkingState = std::make_unique<Player_Walk>(
		this, pCamera, pKeyboard, info.walk_speed, info.walk_speed * 0.5f);

	// 通常攻撃状態を生成
	Player_AttackBasic::AttackParam basicParam =
	{
		info.attack[static_cast<int>(AttackType::BASIC)].time,
		info.attack[static_cast<int>(AttackType::BASIC)].size,
		info.attack[static_cast<int>(AttackType::BASIC)].force,
	};
	m_basicAttackingState = std::make_unique<Player_AttackBasic>(
		this, pKeyboard,basicParam);

	// 転がり攻撃状態を生成
	Player_AttackRolling::AttackParam rollingParam =
	{
		info.attack[static_cast<int>(AttackType::ROLLING)].time,
		info.attack[static_cast<int>(AttackType::ROLLING)].size,
		info.attack[static_cast<int>(AttackType::ROLLING)].force,
		info.dash_speed, info.dash_speed * 0.5f
	};
	m_rollingAttackingState = std::make_unique<Player_AttackRolling>(
		this, pCamera, pKeyboard, rollingParam);

	// 強攻撃状態を生成
	Player_AttackHeavy::AttackParam heavyParam =
	{
		info.attack[static_cast<int>(AttackType::HEAVY)].time,
		info.attack[static_cast<int>(AttackType::HEAVY)].size,
		info.attack[static_cast<int>(AttackType::HEAVY)].force,
		0.6f
	};
	m_heavyAttackingState = std::make_unique<Player_AttackHeavy>(
		this, pKeyboard, heavyParam);

	// 初期状態を設定する
	m_currentState = m_idlingState.get();
	m_currentState->Initialize(m_pResourceManager);
}



/**
 * @brief モデル・アニメーションの設定
 *
 * @param pRM	リソースマネージャーのポインタ
 * @param info	プレイヤー情報
 *
 * @return なし
 */
void Player::SetupModels(ResourceManager* pRM, const PlayerInfoLoader::PlayerInfo& info)
{
	// モデルの読み込み
	m_model = pRM->RequestSDKMESH("player", info.modelPath, true);

	// アニメーションの読み込み
	m_animations = std::make_unique<Animations>();
	m_animations->idle = pRM->RequestAnimation("playerIdle", info.idleAnimPath);
	m_animations->walk = pRM->RequestAnimation("playerWalk", info.walkAnimPath);
	m_animations->atk_basic = pRM->RequestAnimation("playerAtkB",
		info.attackAnimPath[static_cast<int>(AttackType::BASIC)]);
	m_animations->atk_rolling = pRM->RequestAnimation("playerAtkR",
		info.attackAnimPath[static_cast<int>(AttackType::ROLLING)]);
	m_animations->atk_heavy = pRM->RequestAnimation("playerAtkH",
		info.attackAnimPath[static_cast<int>(AttackType::HEAVY)]);
}
