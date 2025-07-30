/**
 * @file   GroundEnemy_Bounce.cpp
 *
 * @brief  GroundEnemy_Bounceに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Bounce.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GroundEnemy_Bounce::GroundEnemy_Bounce(GroundEnemy* groundEnemy)
	:m_pGroundEnemy{ groundEnemy }
{

}


/**
 * @brief デストラクタ
 */
GroundEnemy_Bounce::~GroundEnemy_Bounce()
{

}


void GroundEnemy_Bounce::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->GetModel("enemy");

	m_pGroundEnemy->SetIsAttack(false);
}


void GroundEnemy_Bounce::Update(const float& elapsedTime)
{
	// 座標の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	m_pGroundEnemy->SetOnGround(false);

	if (m_pGroundEnemy->GetVelocity().Length() < 1.0f)	//何故か吹っ飛ばされ状態になって戻ってこないことがある
	{
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Idle());
	}
}


void GroundEnemy_Bounce::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states,
		world, context.view, context.projection);
}


void GroundEnemy_Bounce::Finalize()
{

}
