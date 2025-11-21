/**
 * @file   GroundEnemy_Bounce.cpp
 *
 * @brief  地上の敵の跳ね返り状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Bounce.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param groundEnemy 敵のポインタ
 */
GroundEnemy_Bounce::GroundEnemy_Bounce(GroundEnemy* groundEnemy)
	: m_pGroundEnemy{ groundEnemy }
	, m_stateType{ StateType::Bounce }
{
}



/**
 * @brief 初期化処理
 *
 * @param pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy_Bounce::Initialize(ResourceManager* pRM)
{
	// 継承して未使用の引数
	UNREFERENCED_PARAMETER(pRM);

	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			m_pGroundEnemy->GetModel(),
			m_pGroundEnemy->GetAnimation()->idle
		);
	}
	m_modelAnimator->Initialize();

	m_pGroundEnemy->SetIsAttack(false);
	m_pGroundEnemy->SetAttackCollisionEnabled(false);

	// 軌跡エフェクトの出現をオンにする
	m_pGroundEnemy->GetTrajectoryParticle()->SetSpawn(true);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy_Bounce::Update(const float& elapsedTime)
{
	// 位置の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime);
	m_pGroundEnemy->LimitVelocity();
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 角速度による回転
	RotateAngVel(elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	if (m_pGroundEnemy->GetVelocity().Length() < 1.0f)
	{
		m_pGroundEnemy->GetTrajectoryParticle()->SetSpawn(false);

		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Idle());
	}
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void GroundEnemy_Bounce::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY() + DirectX::XM_PIDIV2);
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
void GroundEnemy_Bounce::Finalize()
{
	if (m_modelAnimator) m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}


/**
 * @brief 角速度による回転
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy_Bounce::RotateAngVel(float elapsedTime)
{
	if (m_pGroundEnemy->GetAngVelocityY() == 0.0f) return;

	m_pGroundEnemy->SetRotY(m_pGroundEnemy->GetRotY() + m_pGroundEnemy->GetAngVelocityY() * elapsedTime);
	m_pGroundEnemy->SetAngVelocityY(m_pGroundEnemy->GetAngVelocityY() * ANGULAR_VELOCITY_DAMPING);

	if (m_pGroundEnemy->GetAngVelocityY() < 0.01f)
	{
		m_pGroundEnemy->SetAngVelocityY(0.0f);
	}
}
