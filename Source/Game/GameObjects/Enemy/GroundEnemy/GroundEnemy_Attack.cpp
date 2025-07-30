/**
 * @file   GroundEnemy_Attack.cpp
 *
 * @brief  GroundEnemy_Attackに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Attack.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GroundEnemy_Attack::GroundEnemy_Attack(GroundEnemy* groundEnemy)
	:m_pGroundEnemy{ groundEnemy }
{

}


/**
 * @brief デストラクタ
 */
GroundEnemy_Attack::~GroundEnemy_Attack()
{

}


void GroundEnemy_Attack::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->GetModel("enemy");

	m_attackTime = ATTACK_TIME;

	m_pGroundEnemy->SetAttackForce(ATTACK_FORCE);

	float rot = m_pGroundEnemy->GetRotY() - XM_PIDIV2;	//モデルの向きの関係で調整
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * m_pGroundEnemy->GetRadius());
	m_pGroundEnemy->GetAttackCollider()->SetRadius(ATTACK_SIZE);
}


void GroundEnemy_Attack::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	// 座標の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	// 攻撃判定の更新
	float rot = m_pGroundEnemy->GetRotY() - XM_PIDIV2;	//モデルの向きの関係で調整
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * m_pGroundEnemy->GetRadius());

	m_pGroundEnemy->SetOnGround(false);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pGroundEnemy->SetIsAttack(false);
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Idle());
	}
}


void GroundEnemy_Attack::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states,
		world, context.view, context.projection);

	if (m_pGroundEnemy->GetSpherePrimitive())
	{
		DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetAttackCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pGroundEnemy->GetSpherePrimitive()->Draw(world, context.view, context.projection, DirectX::Colors::Red, nullptr, true);
	}
}


void GroundEnemy_Attack::Finalize()
{

}
