/**
 * @file   Player_AttackHeavy.cpp
 *
 * @brief  Player_AttackHeavyに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackHeavy.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player_AttackHeavy::Player_AttackHeavy(Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ Player }
	, m_pKbTracker{ kbTracker }
	, m_model{ nullptr }
	, m_attackTime{ 0.0f }
	, m_stateType{ StateType::Attack }
{

}


/**
 * @brief デストラクタ
 */
Player_AttackHeavy::~Player_AttackHeavy()
{

}


void Player_AttackHeavy::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/player.sdkmesh");

	// 力の設定
	m_pPlayer->SetAttackForce(ATTACK_FORCE);

	// 時間の設定
	m_attackTime = ATTACK_TIME;

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(
		sinf(m_pPlayer->GetRotY()), m_pPlayer->GetRadius(), cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);
}


void Player_AttackHeavy::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	// 座標の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(
		sinf(m_pPlayer->GetRotY()), m_pPlayer->GetRadius(), cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());

	m_pPlayer->SetOnGround(false);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->SetAttackCollisionEnabled(false);
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}
}


void Player_AttackHeavy::Render(RenderContext& context)
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
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.projection, Colors::Red, nullptr, true);
	}
}


void Player_AttackHeavy::Finalize()
{

}
