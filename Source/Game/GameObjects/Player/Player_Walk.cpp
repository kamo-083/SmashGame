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
 * @param kbTracker	  キーボードトラッカーのポインタ
 * @param groundSpeed 地上の移動速度
 * @param airSpeed    空中の移動速度
 */
Player_Walk::Player_Walk(
	Player* player, Camera* camera,
	DirectX::Keyboard::KeyboardStateTracker* kbTracker,
	float groundSpeed, float airSpeed)
	: GROUND_SPEED(groundSpeed)
	, AIR_SPEED(airSpeed)
	, m_pPlayer{ player }
	, m_pKbTracker{ kbTracker }
	, m_pCamera{ camera }
	, m_stateType{ StateType::Walk }
{

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
	m_modelAnimator->Initialize();
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
	DirectX::SimpleMath::Vector3 inputVelocity = DirectX::SimpleMath::Vector3::Zero;

	//入力による移動速度
	inputVelocity = m_pPlayer->MoveDirection(m_pKbTracker, m_pCamera);
	if (m_pPlayer->GetPhysics()->IsOnGround())	inputVelocity *= GROUND_SPEED;
	else										inputVelocity *= AIR_SPEED;

	// 入力が無いときは以前の速度を入れる
	if (inputVelocity.LengthSquared() == 0.0f) inputVelocity = m_pPlayer->GetVelocity();

	// 位置の更新
	m_pPlayer->SetVelocity({ inputVelocity.x, m_pPlayer->GetVelocity().y, inputVelocity.z });
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMaxSpeed());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	// 攻撃の切り替え
	m_pPlayer->ChangeAttack(m_pKbTracker);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (!m_pPlayer->PressMoveKey(m_pKbTracker))
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Idle());
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
void Player_Walk::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	DirectX::SimpleMath::Matrix rot =	DirectX::SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pPlayer->GetScale());
	world = scale * rot * trans;

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
	if (m_modelAnimator)m_modelAnimator->Finalize();
	m_modelAnimator.reset();
}
