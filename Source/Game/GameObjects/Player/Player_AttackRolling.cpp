/**
 * @file   Player_AttackRolling.cpp
 *
 * @brief  Player_AttackRollingに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackRolling.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] player	プレイヤーのポインタ
 * @param[in] camera	カメラのポインタ
 * @param[in] kbTracker キーボードトラッカーのポインタ
 */
Player_AttackRolling::Player_AttackRolling(Player* Player, Camera* camera, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ Player }
	, m_pKbTracker{ kbTracker }
	, m_pCamera{ camera }
	, m_attackTime{ 0.0f }
	, m_force{ SimpleMath::Vector3::Zero }
	, m_stateType{ StateType::Attack }
	, m_model{ nullptr }
{

}


/**
 * @brief デストラクタ
 */
Player_AttackRolling::~Player_AttackRolling()
{

}


/**
 * @brief 初期化処理
 *
 * @param[in] pResourceManager  リソースマネージャーのポインタ
 *
 * @return なし
 */
void Player_AttackRolling::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/player.sdkmesh");

	// 力の設定
	m_pPlayer->SetAttackForce(ATTACK_FORCE);

	// 時間の設定
	m_attackTime = ATTACK_TIME;

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);

	// 移動力の初期化
	m_force = SimpleMath::Vector3::Zero;
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void Player_AttackRolling::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	DirectX::SimpleMath::Vector3 inputVelocity = DirectX::SimpleMath::Vector3::Zero;

	//入力による移動速度
	inputVelocity = m_pPlayer->MoveDirection(m_pKbTracker, m_pCamera);
	if (m_pPlayer->GetOnGround())	inputVelocity *= GROUND_SPEED;
	else							inputVelocity *= AIR_SPEED;
	m_pPlayer->LimitVelocity(inputVelocity);

	if (inputVelocity.LengthSquared() != 0.0f) m_force = inputVelocity;

	// 座標の更新
	m_pPlayer->SetVelocity(m_force);
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), MAX_SPEED);
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->SetAttackCollisionEnabled(false);
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}
}


/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 *
 * @return なし
 */
void Player_AttackRolling::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection);

	if (m_pPlayer->GetSpherePrimitive())
	{
		scale = SimpleMath::Matrix::CreateScale(m_pPlayer->GetAttackCollider()->GetRadius());
		trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.projection, Colors::Green, nullptr, true);
	}
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player_AttackRolling::Finalize()
{

}
