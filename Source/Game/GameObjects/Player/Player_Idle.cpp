/**
 * @file   Player_Idle.cpp
 *
 * @brief  プレイヤーの待機状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Idle.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 * @param kbTracker キーボードトラッカーのポインタ
 */
Player_Idle::Player_Idle(Player* player, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: 
	m_pPlayer{ player },
	m_pKbTracker{ kbTracker },
	m_stateType{ StateType::Idle }
{

}



/**
 * @brief 初期化処理
 *
 * @param pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void Player_Idle::Initialize(ResourceManager* pRM)
{
	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			pRM->GetModel("player"),
			m_pPlayer->GetAnimation()->idle
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
void Player_Idle::Update(const float& elapsedTime)
{
	PhysicsObject* physics = m_pPlayer->GetPhysics();

	// 位置の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃の切り替え
	m_pPlayer->ChangeAttack(m_pKbTracker);

	// 吹っ飛ばされ状態
	if (m_pPlayer->GetIsBounce())
	{
		// 角速度による回転
		float angVel;
		if (physics->GetAngVelocity().x < physics->GetAngVelocity().z) angVel = physics->GetAngVelocity().z;
		else angVel = physics->GetAngVelocity().x;
		m_pPlayer->SetRotY(m_pPlayer->GetRotY() + angVel * elapsedTime);


		if (m_pPlayer->GetVelocity().Length() < 1.0f)
		{
			m_pPlayer->SetIsBounce(false);
			m_pPlayer->GetTrajectoryParticle()->SetSpawn(false);
		}
		else return;
	}

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 歩き状態に切り替え
	if (m_pPlayer->PressMoveKey(m_pKbTracker))
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}

	// 攻撃状態に切り替え
	if (m_pKbTracker->IsKeyPressed(m_pPlayer->GetKeyConfig().attack))
	{
		m_pPlayer->Attack();
	}
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void Player_Idle::Render(const RenderContext& context)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix rot =	DirectX::SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

	// モデルの描画
	m_modelAnimator->Draw(context, world);

	// 当たり判定のデバッグ描画
	if (m_pPlayer->GetSpherePrimitive())
	{
		scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetCollider()->GetRadius());
		trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetCollider()->GetCenter());
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
void Player_Idle::Finalize()
{
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
