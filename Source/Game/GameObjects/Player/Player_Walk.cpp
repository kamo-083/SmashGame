/**
 * @file   Player_Walk.cpp
 *
 * @brief  プレイヤーの移動状態に関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Walk.h"
#include"Player.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	  プレイヤーのポインタ
 * @param camera	  カメラのポインタ
 * @param groundSpeed 地上の移動速度
 * @param airSpeed    空中の移動速度
 */
Player_Walk::Player_Walk(
	Player* player, Camera* camera,
	float groundSpeed, float airSpeed)
	:
	GROUND_SPEED(groundSpeed),
	AIR_SPEED(airSpeed),
	m_pPlayer{ player },
	m_pCamera{ camera },
	m_stateType{ StateType::Walk },
	m_direction{}
{
}

/**
 * @brief デストラクタ
 *
 * @param なし
 */
Player_Walk::~Player_Walk()
{
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}

/**
 * @brief 初期化処理
 *
 * @param pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void Player_Walk::Initialize(ResourceManager* pRM)
{
	// モデルアニメーターの作成
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			pRM->GetModel("player"),
			m_pPlayer->GetAnimation()->walk
		);
	}
	m_modelAnimator->Initialize(FLT_MAX, true, ANIMATION_SPEED);
	m_modelAnimator->Play();
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Player_Walk::Update(const float& elapsedTime)
{
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
		m_inputVelocity = m_pPlayer->MoveDirection(move_x, move_z, m_pCamera);
		// 速度を計算
		if (m_pPlayer->GetPhysics()->IsOnGround())	m_inputVelocity *= GROUND_SPEED;
		else										m_inputVelocity *= AIR_SPEED;
	}
	// 入力が無いときは以前の速度を入れる
	if (m_inputVelocity.LengthSquared() == 0.0f) m_inputVelocity = m_pPlayer->GetVelocity();

	// 位置の更新
	m_pPlayer->SetVelocity({ m_inputVelocity.x, m_pPlayer->GetVelocity().y, m_inputVelocity.z });
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

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
void Player_Walk::Render(const RenderContext& context)
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
void Player_Walk::Finalize()
{
}

/**
 * @brief メッセージを処理
 *
 * @param messageID メッセージID
 *
 * @return なし
 */
void Player_Walk::OnMessage(Message::MessageID messageID)
{
	switch (messageID)
	{
	case Message::MessageID::PLAYER_IDLE:
		// 待機
		m_inputVelocity = DirectX::SimpleMath::Vector3::Zero;
		m_pPlayer->ChangeState(m_pPlayer->GetState_Idle());
		break;
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
	case Message::MessageID::PLAYER_ATTACK:
		// 攻撃
		m_inputVelocity = DirectX::SimpleMath::Vector3::Zero;
		m_pPlayer->Attack();
		break;
	case Message::MessageID::ATTACK_CHANGE_LEFT:
	case Message::MessageID::ATTACK_CHANGE_RIGHT:
		// 攻撃変更
		m_pPlayer->ChangeAttack(messageID);
		break;
	}
}