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
	:
	m_pGroundEnemy{ groundEnemy },
	ATTACK_TIME{ info.attack.time },
	ATTACK_SIZE{ info.attack.size },
	ATTACK_FORCE{ info.attack.force },
	DISTANCE_RAITO{ info.attack.distance_raito },
	m_stateType{ StateType::Attack }
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
	// 継承して未使用の引数
	UNREFERENCED_PARAMETER(pRM);

	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			m_pGroundEnemy->GetModel(),
			m_pGroundEnemy->GetAnimation()->attack
		);
	}
	m_modelAnimator->Initialize(ATTACK_TIME, false, ANIMATION_SPEED);
	m_modelAnimator->Play();

	// 攻撃力を初期化
	m_pGroundEnemy->SetAttackForce(ATTACK_FORCE);

	// 攻撃判定の設定
	float rot = m_pGroundEnemy->GetRotY();
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * (m_pGroundEnemy->GetRadius() * DISTANCE_RAITO));
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
	// 位置の更新
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime);
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	// 攻撃判定の更新
	float rot = m_pGroundEnemy->GetRotY();
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(rot), 0.0f, cosf(rot));
	m_pGroundEnemy->GetAttackCollider()->SetCenter(m_pGroundEnemy->GetPosition() - forward * (m_pGroundEnemy->GetRadius() * DISTANCE_RAITO));

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (!m_modelAnimator->IsPlaying())
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
void GroundEnemy_Attack::Render(const RenderContext& context)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	DirectX::SimpleMath::Matrix rot =	DirectX::SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY()+ DirectX::XM_PIDIV2);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetScale());
	world = scale * rot * trans;

	// モデルの描画
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