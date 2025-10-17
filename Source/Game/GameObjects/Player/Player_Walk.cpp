/**
 * @file   Player_Walk.cpp
 *
 * @brief  Player_Walkに関するソースファイル
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Walk.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param player	プレイヤーのポインタ
 * @param camera	カメラのポインタ
 * @param kbTracker キーボードトラッカーのポインタ
 */
Player_Walk::Player_Walk(Player* player, Camera* camera, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ player }
	, m_pKbTracker{ kbTracker }
	, m_pCamera{ camera }
	, m_stateType{ StateType::Walk }
{

}


/**
 * @brief デストラクタ
 */
Player_Walk::~Player_Walk()
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
	if (m_pPlayer->GetOnGround())	inputVelocity *= GROUND_SPEED;
	else							inputVelocity *= AIR_SPEED;

	// 座標の更新
	m_pPlayer->SetVelocity(m_pPlayer->GetVelocity() + inputVelocity);
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// 武器の切り替え
	m_pPlayer->ChangeWeapon(m_pKbTracker);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 待機状態に切り替え
	if (!m_pKbTracker->GetLastState().W && !m_pKbTracker->GetLastState().S && !m_pKbTracker->GetLastState().A && !m_pKbTracker->GetLastState().D)
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Idle());
	}

	// 攻撃状態に切り替え
	if (m_pKbTracker->pressed.Space)
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
	m_modelAnimator.reset();
}
