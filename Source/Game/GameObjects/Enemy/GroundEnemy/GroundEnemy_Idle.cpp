/**
 * @file   GroundEnemy_Idle.cpp
 *
 * @brief  GroundEnemy_Idleに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Idle.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GroundEnemy_Idle::GroundEnemy_Idle(GroundEnemy* groundEnemy)
	:m_pGroundEnemy{groundEnemy}
{

}


/**
 * @brief デストラクタ
 */
GroundEnemy_Idle::~GroundEnemy_Idle()
{

}


void GroundEnemy_Idle::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->GetModel("enemy");
}


void GroundEnemy_Idle::Update(const float& elapsedTime)
{
	// 座標の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	m_pGroundEnemy->SetOnGround(false);

	// 歩き状態に切り替え
	if (m_pGroundEnemy->GetPlayerRelativeData().distance <= GroundEnemy::DitectionRange)
	{
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Walk());
	}
}


void GroundEnemy_Idle::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states,
				  world, context.view, context.projection);
}


void GroundEnemy_Idle::Finalize()
{

}
