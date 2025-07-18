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
	SimpleMath::Vector3 force = m_pGroundEnemy->GetPlayerRelativeData().direction * MOVE_SPEED;

	// 座標の更新
	m_pGroundEnemy->SetVelocity(m_pGroundEnemy->GetVelocity() + force * elapsedTime);
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
}


void GroundEnemy_Walk::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	world = trans;

	m_model->Draw(context.deviceContext, *context.states,
		world, context.view, context.projection, true);
}


void GroundEnemy_Walk::Finalize()
{

}
