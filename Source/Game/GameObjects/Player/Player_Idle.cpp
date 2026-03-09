/**
 * @file   Player_Idle.cpp
 *
 * @brief  プレイヤーの待機状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Idle.h"
#include"Player.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 */
Player_Idle::Player_Idle(Player* player)
	: 
	m_pPlayer{ player },
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
	m_modelAnimator->Play();
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



/**
 * @brief メッセージを処理
 *
 * @param messageID メッセージID
 *
 * @return なし
 */
void Player_Idle::OnMessage(Message::MessageID messageID)
{
	switch (messageID)
	{
	case Message::MessageID::PLAYER_MOVE_FORWARD:
	case Message::MessageID::PLAYER_MOVE_BACKWARD:
	case Message::MessageID::PLAYER_MOVE_LEFT:
	case Message::MessageID::PLAYER_MOVE_RIGHT:
		// 吹っ飛ばされていなければ移動
		if(!m_pPlayer->GetIsBounce()) m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
		break;
	case Message::MessageID::PLAYER_ATTACK:
		// 吹っ飛ばされていなければ攻撃
		if (!m_pPlayer->GetIsBounce()) m_pPlayer->Attack();
		break;
	case Message::MessageID::ATTACK_CHANGE_LEFT:
	case Message::MessageID::ATTACK_CHANGE_RIGHT:
		// 攻撃変更
		m_pPlayer->ChangeAttack(messageID);
		break;
	}
}
