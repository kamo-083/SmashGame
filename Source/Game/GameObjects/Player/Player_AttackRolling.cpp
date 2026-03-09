/**
 * @file   Player_AttackRolling.cpp
 *
 * @brief  プレイヤーの転がり攻撃状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_AttackRolling.h"
#include"Player.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	  プレイヤーのポインタ
 * @param camera	  カメラのポインタ
 * @param param		  定数初期化用パラメータ
 */
Player_AttackRolling::Player_AttackRolling(
	Player* player, Camera* camera,
	const AttackParam& param)
	: 
	ATTACK_TIME(param.time),
	ATTACK_SIZE(param.size),
	ATTACK_FORCE(param.force),
	GROUND_SPEED(param.groundSpeed),
	AIR_SPEED(param.airSpeed),
	m_pPlayer{ player },
	m_pCamera{ camera },
	m_attackTime{ 0.0f },
	m_moveForce{ DirectX::SimpleMath::Vector3::Zero },
	m_stateType{ StateType::Attack },
	m_direction{}
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
	m_modelAnimator->Initialize(ANIM_TIME, true, ANIMATION_SPEED);
	m_modelAnimator->Play();

	// 攻撃力・攻撃時間の初期化
	m_pPlayer->SetAttackForce(ATTACK_FORCE);
	m_attackTime = ATTACK_TIME;

	// 攻撃判定の設定
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());
	m_pPlayer->GetAttackCollider()->SetRadius(m_pPlayer->GetRadius() * ATTACK_SIZE);

	// 移動力の初期化 (向いている方向に設定)
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(-(sinf(m_pPlayer->GetRotY())), 0.0f, -(cosf(m_pPlayer->GetRotY())));
	forward.Normalize();
	m_moveForce = forward * GROUND_SPEED;
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
	// 攻撃時間を更新
	m_attackTime -= elapsedTime;

	// 移動方向
	int move_x = 0;	// 左右
	int move_z = 0;	// 前後
	// 前後
	if (m_direction.forward)	   move_z--;
	else if (m_direction.backward) move_z++;
	// 左右
	if (m_direction.left)	    move_x--;
	else if (m_direction.right) move_x++;

	if (move_x != 0 || move_z != 0)
	{
		// 移動方向を決定
		m_moveForce = m_pPlayer->MoveDirection(move_x, move_z, m_pCamera);
		// 速度を計算
		if (m_pPlayer->GetPhysics()->IsOnGround())	m_moveForce *= GROUND_SPEED;
		else										m_moveForce *= AIR_SPEED;
	}

	// 位置の更新
	m_pPlayer->SetVelocity({ m_moveForce.x, m_pPlayer->GetVelocity().y, m_moveForce.z });
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃判定の更新
	m_pPlayer->GetAttackCollider()->SetCenter(m_pPlayer->GetPosition());

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (m_attackTime <= 0.0f)
	{
		m_pPlayer->SetIsAttack(false);
		m_pPlayer->SetAttackCollisionEnabled(false);
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}

	// 入力情報を初期化
	m_direction.Reset();
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void Player_AttackRolling::Render(const RenderContext& context)
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



/**
 * @brief メッセージを処理
 *
 * @param messageID メッセージID
 *
 * @return なし
 */
void Player_AttackRolling::OnMessage(Message::MessageID messageID)
{
	switch (messageID)
	{
	case Message::MessageID::PLAYER_MOVE_FORWARD:
		// 前へ
		m_direction.forward = true;
		break;
	case Message::MessageID::PLAYER_MOVE_BACKWARD:
		// 後ろへ
		m_direction.backward = true;
		break;
	case Message::MessageID::PLAYER_MOVE_LEFT:
		// 左へ
		m_direction.left = true;
		break;
	case Message::MessageID::PLAYER_MOVE_RIGHT:
		// 右へ
		m_direction.right = true;
		break;
	}
}
