/**
 * @file   Player_AttackRolling.cpp
 *
 * @brief  プレイヤーの転がり攻撃状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackRolling.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 * @param camera	カメラのポインタ
 * @param kbTracker キーボードトラッカーのポインタ
 */
Player_AttackRolling::Player_AttackRolling(Player* Player, Camera* camera, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ Player }
	, m_pKbTracker{ kbTracker }
	, m_pCamera{ camera }
	, m_attackTime{ 0.0f }
	, m_force{ DirectX::SimpleMath::Vector3::Zero }
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
void Player_AttackRolling::Initialize(ResourceManager* pRM)
{
	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			pRM->GetModel("player"),
			m_pPlayer->GetAnimation()->atk_rolling
		);
	}
	m_modelAnimator->Initialize(ANIM_TIME);

	// 力の設定
	m_pPlayer->SetAttackForce(ATTACK_FORCE);

	// 時間の設定
	m_attackTime = ATTACK_TIME;

	// 攻撃判定の設定
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);

	// 移動力の初期化
	m_force = DirectX::SimpleMath::Vector3::Zero;
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Player_AttackRolling::Update(const float& elapsedTime)
{
	m_attackTime -= elapsedTime;

	DirectX::SimpleMath::Vector3 inputVelocity = DirectX::SimpleMath::Vector3::Zero;

	//入力による移動速度
	inputVelocity = m_pPlayer->MoveDirection(m_pKbTracker, m_pCamera);
	if (m_pPlayer->GetOnGround())	inputVelocity *= GROUND_SPEED;
	else							inputVelocity *= AIR_SPEED;
	m_pPlayer->LimitVelocity(inputVelocity, m_pPlayer->GetMaxSpeed());

	if (inputVelocity.LengthSquared() != 0.0f) m_force = inputVelocity;

	// 位置の更新
	m_pPlayer->SetVelocity(m_force);
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), MAX_SPEED);
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(sinf(m_pPlayer->GetRotY()), 0.0f, cosf(m_pPlayer->GetRotY()));
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->SetAttackCollisionEnabled(false);
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
void Player_AttackRolling::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	m_modelAnimator->Draw(context, world);

	// 当たり判定のデバッグ描画
	if (m_pPlayer->GetSpherePrimitive())
	{
		scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetAttackCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetAttackCollider()->GetCenter());
		world = scale * trans;
		m_pPlayer->GetSpherePrimitive()->Draw(world, context.view, context.proj, DirectX::Colors::Green, nullptr, true);
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Player_AttackRolling::Finalize()
{
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
