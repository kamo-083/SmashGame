/**
 * @file   GroundEnemy_Attack.cpp
 *
 * @brief  地上の敵の攻撃状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Attack.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param groundEnemy 敵のポインタ
 * @param info		  敵の情報(攻撃関連情報取得のため)
 */
GroundEnemy_Attack::GroundEnemy_Attack(GroundEnemy* groundEnemy, const EnemyInfoLoader::EnemyInfo& info)
	: m_pGroundEnemy{ groundEnemy }
	, ATTACK_TIME{ info.attack.time }
	, ATTACK_SIZE{ info.attack.size }
	, ATTACK_FORCE{ info.attack.force }
	, m_attackTime{ 0.0f }
	, m_stateType{ StateType::Attack }
{

}



/**
 * @brief 初期化処理
 *
 * @param pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy_Attack::Initialize(ResourceManager* pRM)
{
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			m_pGroundEnemy->GetModel(),
			m_pGroundEnemy->GetAnimation()->attack
		);
	}
	m_modelAnimator->Initialize(ATTACK_TIME);

	m_attackTime = ATTACK_TIME;

	m_pGroundEnemy->SetAttackForce(ATTACK_FORCE);

	float rot = m_pGroundEnemy->GetRotY() - DirectX::XM_PIDIV2;	//モデルの向きの関係で調整
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * (m_pGroundEnemy->GetRadius() * 0.5f));
	m_pGroundEnemy->GetAttackCollider()->SetRadius(ATTACK_SIZE);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy_Attack::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	// 座標の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	// 攻撃判定の更新
	float rot = m_pGroundEnemy->GetRotY() - DirectX::XM_PIDIV2;	//モデルの向きの関係で調整
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * (m_pGroundEnemy->GetRadius() * 0.5f));

	m_pGroundEnemy->SetOnGround(false);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pGroundEnemy->SetIsAttack(false);
		m_pGroundEnemy->SetAttackCollisionEnabled(false);
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
void GroundEnemy_Attack::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	DirectX::SimpleMath::Matrix rot =	DirectX::SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetScale());
	world = scale * rot * trans;

	m_modelAnimator->Draw(context, world);

	// 当たり判定のデバッグ描画
	if (m_pGroundEnemy->GetSpherePrimitive())
	{
		DirectX::SimpleMath::Matrix sphereScale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetAttackCollider()->GetRadius()*2.0f);
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetAttackCollider()->GetCenter());
		world = sphereScale * trans;
		m_pGroundEnemy->GetSpherePrimitive()->Draw(world, context.view, context.proj, DirectX::Colors::Red, nullptr, true);
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void GroundEnemy_Attack::Finalize()
{
	if (m_modelAnimator) m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
