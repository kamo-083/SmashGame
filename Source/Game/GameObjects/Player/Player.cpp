/**
 * @file   Player.cpp
 *
 * @brief  Playerに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player::Player()
{

}

Player::Player(ID3D11DeviceContext* context)
{
	m_sphere = DirectX::GeometricPrimitive::CreateSphere(context);
}


/**
 * @brief デストラクタ
 */
Player::~Player()
{

}


void Player::Initialize(ResourceManager* pResourceManager, 
						DirectX::Keyboard::KeyboardStateTracker* pKbTracker,
						Camera* pCamera, 
						WeaponUI* weaponUI,
						bool* keyMode)
{
	// 座標の初期化
	m_position = SimpleMath::Vector3::Zero;
	m_position.y = 2.0f;

	// 速度の初期化
	m_velocity = SimpleMath::Vector3::Zero;

	// 向きの初期化
	m_rotY = 0.0f;

	// 着地
	m_onGround = false;

	// 吹っ飛ばされ状態
	m_isBounce = false;

	// 武器の設定
	m_weaponType = WeaponType::BASIC;

	// 武器UIの設定
	m_pWeaponUI = weaponUI;

	// リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	// モデルの読み込み
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources\\Models\\player_cat.sdkmesh");

	// コライダーの設定
	m_collider = SphereCollider(m_position, RADIUS);

	// 攻撃判定の設定
	m_attackCollider = SphereCollider(m_position, RADIUS);

	// キー操作のモードのポインタの設定
	m_pKeyMode = keyMode;

	// 物理演算の設定
	m_physics = std::make_unique<PhysicsObject>();
	m_physics->GetFriction().SetDynamicFriction(1.3f);

	// 待機状態を生成
	m_idlingState = std::make_unique<Player_Idle>(this, pKbTracker);
	// 待機状態を初期化
	m_idlingState->Initialize(pResourceManager);

	// 歩き状態を生成
	m_walkingState = std::make_unique<Player_Walk>(this, pCamera, pKbTracker);
	// 歩き状態を初期化
	m_walkingState->Initialize(pResourceManager);

	// 通常攻撃状態を生成
	m_basicAttackingState = std::make_unique<Player_AttackBasic>(this, pKbTracker);
	m_basicAttackingState->Initialize(pResourceManager);

	// 転がり攻撃状態を生成
	m_rollingAttackingState = std::make_unique<Player_AttackRolling>(this, pCamera, pKbTracker);
	m_rollingAttackingState->Initialize(pResourceManager);
	
	// 転がり攻撃状態を生成
	m_heavyAttackingState = std::make_unique<Player_AttackHeavy>(this, pKbTracker);
	m_heavyAttackingState->Initialize(pResourceManager);

	// 初期状態を設定する
	m_currentState = m_idlingState.get();
}


void Player::Update(const float& elapsedTime)
{
	// 現在の状態を更新する
	m_currentState->Update(elapsedTime);
}


void Player::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	// 現在の状態を描画する
	m_currentState->Render(context);

	debugFont->AddString(0, 60, DirectX::Colors::Cyan, L"pos = %f,%f,%f", m_position.x, m_position.y, m_position.z);
	debugFont->AddString(0, 85, DirectX::Colors::Cyan, L"vel = %f,%f,%f", m_velocity.x, m_velocity.y, m_velocity.z);
	debugFont->AddString(0, 110, DirectX::Colors::Cyan, L"weapon = %d", static_cast<int>(m_weaponType));
	debugFont->AddString(140, 110, DirectX::Colors::Cyan, L"onGround = %d", static_cast<int>(m_onGround));
}


void Player::Finalize()
{
	// 状態をリセットする
	m_idlingState.reset();
}


void Player::ChangeState(IState* newState)
{
	// 新規の状態を現在の状態に設定する
	m_currentState = newState;

	// 状態を初期化
	m_currentState->Initialize(m_pResourceManager);
}


void Player::ChangeWeapon(DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	if (!(*m_pKeyMode)) return;

	if (pKbTracker->pressed.J)
	{
		++m_weaponType;
	}
	else if (pKbTracker->pressed.L)
	{
		--m_weaponType;
	}

	m_pWeaponUI->ChangeWeapon(m_weaponType);
}

void Player::Attack()
{
	m_isAttack = true;
	switch (m_weaponType)
	{
	case WeaponType::BASIC:
		ChangeState(m_basicAttackingState.get());
		break;
	case WeaponType::ROLLING:
		ChangeState(m_rollingAttackingState.get());
		break;
	case WeaponType::HEAVY:
		ChangeState(m_heavyAttackingState.get());
		break;
	default:
		break;
	}
}

DirectX::SimpleMath::Vector3 Player::MoveDirection(DirectX::Keyboard::KeyboardStateTracker* kbTracker,
												   Camera* camera)
{
	DirectX::SimpleMath::Vector3 forward = camera->GetForward();
	DirectX::SimpleMath::Vector3 right = forward.Cross(camera->GetUp());
	DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::Zero;

	//移動
	if (kbTracker->GetLastState().S)
	{
		direction += -forward;
	}
	else if (kbTracker->GetLastState().W)
	{
		direction -= -forward;
	}
	if (kbTracker->GetLastState().D)
	{
		direction += right;
	}
	else if (kbTracker->GetLastState().A)
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

void Player::LimitVelocity(DirectX::SimpleMath::Vector3& velocity, float max)
{
	velocity.x = std::min(std::max(velocity.x, -max), max);
	velocity.y = std::min(std::max(velocity.y, -MAX_SPEED), MAX_SPEED);
	velocity.z = std::min(std::max(velocity.z, -max), max);
}

bool Player::DetectCollisionToBox(OBBCollider collider)
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

		OBBCollider::CollisionType type = DetermineCollisionType(normal);
		switch (type)
		{
		case OBBCollider::CollisionType::Ground:
			m_velocity.y = 0.0f;
			m_onGround = true;
			break;
		case OBBCollider::CollisionType::Wall:
			if (abs(normal.x) > abs(normal.z)) m_velocity.x = 0.0f;
			else							   m_velocity.z = 0.0f;
			break;
		default:
			break;
		}
	}

	return hit;
}

bool Player::DetectCollisionToSphere(SphereCollider collider)
{
	bool hit = IsHit(collider, m_collider);

	if (hit)
	{
		MTV mtv = CalculateMTV(collider, m_collider);
		if (mtv.distance > 0.0f && mtv.direction.LengthSquared() > 0.00001f)
		{
			m_position += mtv.direction * mtv.distance;
			m_collider.SetCenter(m_position);
			//SetAttackPosition();
		}
	}

	return hit;
}

bool Player::DetectCollisionToAttack(SphereCollider collider, float power)
{
	bool hit = IsHit(collider, m_collider);

	if (hit)
	{
		MTV mtv = CalculateMTV(collider, m_collider);

		// 吹っ飛ぶ方向の設定
		DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
		knockbackDir.Normalize();

		// 吹っ飛ぶ力の設定
		float knockbackForce = (1.0f + mtv.distance) * power;

		DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
		m_physics->GetExternalForce().Add(force);

		// 跳ね返り状態に遷移
		m_isBounce = true;
		ChangeState(m_idlingState.get());
	}

	return hit;
}
