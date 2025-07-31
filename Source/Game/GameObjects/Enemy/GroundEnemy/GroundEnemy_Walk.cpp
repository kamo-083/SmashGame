/**
 * @file   GroundEnemy_Walk.cpp
 *
 * @brief  GroundEnemy_Walkに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Walk.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GroundEnemy_Walk::GroundEnemy_Walk(GroundEnemy* groundEnemy)
	:m_pGroundEnemy{ groundEnemy }
	,m_stateType{StateType::Walk}
{

}


/**
 * @brief デストラクタ
 */
GroundEnemy_Walk::~GroundEnemy_Walk()
{

}


void GroundEnemy_Walk::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->GetModel("enemy");
}


void GroundEnemy_Walk::Update(const float& elapsedTime)
{
	SimpleMath::Vector3 force = m_pGroundEnemy->GetPlayerRelativeData().direction * MOVE_SPEED * elapsedTime;

	//回転
	if (m_pGroundEnemy->GetPlayerRelativeData().direction.x != 0.0f || m_pGroundEnemy->GetPlayerRelativeData().direction.z != 0.0f)
	{
		m_pGroundEnemy->SetRotY(std::atan2f(-m_pGroundEnemy->GetPlayerRelativeData().direction.x, -m_pGroundEnemy->GetPlayerRelativeData().direction.z));
	}

	// 座標の更新
	m_pGroundEnemy->SetVelocity(m_pGroundEnemy->GetVelocity() + force);
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	m_pGroundEnemy->SetOnGround(false);

	// 待機状態に切り替え
	if (m_pGroundEnemy->GetPlayerRelativeData().distance > GroundEnemy::DitectionRange)
	{
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Idle());
	}

	// 攻撃状態に切り替え
	if (m_pGroundEnemy->GetPlayerRelativeData().distance <= m_pGroundEnemy->GetAttackCollider()->GetRadius())
	{
		m_pGroundEnemy->SetIsAttack(true);
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Attack());
	}
}


void GroundEnemy_Walk::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states,
		world, context.view, context.projection, true);
}


void GroundEnemy_Walk::Finalize()
{

}
