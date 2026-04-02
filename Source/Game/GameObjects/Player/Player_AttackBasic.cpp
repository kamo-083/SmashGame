/**
 * @file   Player_AttackBasic.cpp
 *
 * @brief  プレイヤーの通常攻撃状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackBasic.h"
#include"Player.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 * @param param		定数初期化用パラメータ
 */
Player_AttackBasic::Player_AttackBasic(Player* player,	const AttackParam& param)
	:
	ATTACK_TIME(param.time),
	ATTACK_SIZE(param.size),
	ATTACK_FORCE(param.force),
	m_pPlayer{ player },
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
void Player_AttackBasic::Initialize(ResourceManager* pRM)
{
	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			pRM->GetModel("player"),
			m_pPlayer->GetAnimation()->atk_basic
		);
	}
	m_modelAnimator->Initialize(ATTACK_TIME, false, ANIMATION_SPEED);
	m_modelAnimator->Play();

	// 攻撃力の初期化
	m_pPlayer->SetAttackForce(ATTACK_FORCE);

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Player_AttackBasic::Update(const float& elapsedTime)
{
	// 位置の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition() - forward * m_pPlayer->GetRadius());

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (!m_modelAnimator->IsPlaying())
	{
		m_pPlayer->AttackCancel();
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
void Player_AttackBasic::Render(const RenderContext& context)
{
	// ワールド行列を計算
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix rot =	DirectX::SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	// モデルを描画
	m_modelAnimator->Draw(context, world);

	// 当たり判定のデバッグ描画
	if (m_pPlayer->GetSpherePrimitive())
	{
		scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetAttackCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.proj, DirectX::Colors::Blue, nullptr, true);
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Player_AttackBasic::Finalize()
{
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
