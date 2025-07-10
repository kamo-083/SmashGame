/**
 * @file   Player_AttackBasic.cpp
 *
 * @brief  Player_AttackBasicに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackBasic.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player_AttackBasic::Player_AttackBasic(Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ Player }
	, m_pKbTracker{ kbTracker }
{

}


/**
 * @brief デストラクタ
 */
Player_AttackBasic::~Player_AttackBasic()
{

}


void Player_AttackBasic::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/mouse.sdkmesh");

	// 力の設定
	m_pPlayer->SetAttackForce(ATTACK_FORCE);

	// 時間の設定
	m_attackTime = ATTACK_TIME;

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);
}


void Player_AttackBasic::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	// 座標の更新
	m_pPlayer->GetPhysics()->CalculateVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->GetPhysics()->AddFliction(m_pPlayer->GetVelocity(), m_pPlayer->GetOnGround());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());

	m_pPlayer->SetOnGround(false);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}
}


void Player_AttackBasic::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection);

	if (m_pPlayer->GetSpherePrimitive())
	{
		DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetAttackCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.projection, DirectX::Colors::Blue, nullptr, true);
	}
}


void Player_AttackBasic::Finalize()
{

}
