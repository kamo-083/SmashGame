/**
 * @file   GroundEnemy_Idle.cpp
 *
 * @brief  地上の敵の待機状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Idle.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param groundEnemy 敵のポインタ
 */
GroundEnemy_Idle::GroundEnemy_Idle(GroundEnemy* groundEnemy)
	: m_pGroundEnemy{ groundEnemy }
	, m_stateType{ StateType::Idle }
{

}



/**
 * @brief 初期化処理
 *
 * @param pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy_Idle::Initialize(ResourceManager* pRM)
{
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			m_pGroundEnemy->GetModel(),
			m_pGroundEnemy->GetAnimation()->idle
		);
	}
	m_modelAnimator->Initialize();
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy_Idle::Update(const float& elapsedTime)
{
	// 座標の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	m_pGroundEnemy->SetOnGround(false);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 歩き状態に切り替え
	if (m_pGroundEnemy->GetPlayerRelativeData().distance <= GroundEnemy::DITECTION_RANGE)
	{
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Walk());
	}
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void GroundEnemy_Idle::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetScale());
	world = scale * rot * trans;

	m_modelAnimator->Draw(context, world);
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void GroundEnemy_Idle::Finalize()
{
	if (m_modelAnimator) m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
