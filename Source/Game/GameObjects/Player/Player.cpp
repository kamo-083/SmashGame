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


/**
 * @brief デストラクタ
 */
Player::~Player()
{

}


void Player::Initialize(ResourceManager* pResourceManager, DirectX::Keyboard::KeyboardStateTracker* pKbTracker, Camera* pCamera)
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

	// リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	// モデルの読み込み
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources\\Models\\mouse.sdkmesh");

	// コライダーの設定
	m_collider = SphereCollider(m_position, RADIUS);

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 pos = { m_position.x, m_position.y - RADIUS * 2.0f, m_position.z - RADIUS };
	m_attackCollider.SetRadius(RADIUS * 2.0f);

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
