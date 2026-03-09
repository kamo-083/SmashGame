/**
 * @file   Player_AttackHeavy.cpp
 *
 * @brief  プレイヤーの強攻撃状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackHeavy.h"
#include"Player.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 * @param param		定数初期化用パラメータ
 */
Player_AttackHeavy::Player_AttackHeavy(Player* player, const AttackParam& param)
	:
	ATTACK_TIME(param.time),
	ATTACK_SIZE(param.size),
	ATTACK_FORCE(param.force),
	COOL_TIME(param.cool),
	m_pPlayer{ player },
	m_attackTime{ 0.0f },
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
void Player_AttackHeavy::Initialize(ResourceManager* pRM)
{
	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			pRM->GetModel("player"),
			m_pPlayer->GetAnimation()->atk_heavy
		);
	}
	m_modelAnimator->Initialize(ATTACK_TIME + COOL_TIME, false, ANIMATION_SPEED);
	m_modelAnimator->Play();

	// 攻撃力・攻撃時間の初期化
	m_pPlayer->SetAttackForce(ATTACK_FORCE);
	m_attackTime = ATTACK_TIME + COOL_TIME;

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(
		sinf(m_pPlayer->GetRotY()), m_pPlayer->GetRadius(), cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(DirectX::SimpleMath::Vector3(
		m_pPlayer->GetPosition().x - forward.x * m_pPlayer->GetRadius(),
		m_pPlayer->GetPosition().y - m_pPlayer->GetRadius() * COLLIDER_POS_ADJUST,
		m_pPlayer->GetPosition().z - forward.z * m_pPlayer->GetRadius()));
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Player_AttackHeavy::Update(const float& elapsedTime)
{
	// 攻撃時間の更新
	m_attackTime -= elapsedTime * ANIMATION_SPEED;

	// 位置の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	if (m_attackTime > COOL_TIME)
	{
		DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(
			sinf(m_pPlayer->GetRotY()), m_pPlayer->GetRadius(), cosf(m_pPlayer->GetRotY()));
		m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());
	}
	else
	{
		m_pPlayer->SetAttackCollisionEnabled(false);
	}

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (!m_modelAnimator->IsPlaying())
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void Player_AttackHeavy::Render(const RenderContext& context)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	// モデルの描画
	m_modelAnimator->Draw(context, world);

	// 当たり判定のデバッグ描画
	if (m_pPlayer->GetSpherePrimitive() && m_pPlayer->GetAttackCollisionEnabled())
	{
		scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetAttackCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.proj, DirectX::Colors::Red, nullptr, true);
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Player_AttackHeavy::Finalize()
{
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
