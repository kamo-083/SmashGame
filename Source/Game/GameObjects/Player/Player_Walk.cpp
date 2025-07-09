/**
 * @file   Player_Walk.cpp
 *
 * @brief  Player_Walkに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Walk.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player_Walk::Player_Walk(Player* player, Camera* camera, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ player }
	, m_pKbTracker{ kbTracker }
	, m_pCamera{ camera }
{

}


/**
 * @brief デストラクタ
 */
Player_Walk::~Player_Walk()
{

}


void Player_Walk::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/mouse.sdkmesh");
}


void Player_Walk::Update(const float& elapsedTime)
{
	DirectX::SimpleMath::Vector3 forward = m_pCamera->GetForward();
	DirectX::SimpleMath::Vector3 right = forward.Cross(m_pCamera->GetUp());
	DirectX::SimpleMath::Vector3 force = DirectX::SimpleMath::Vector3::Zero;

	//移動
	float speed = 0.0f;
	if (m_pPlayer->GetOnGround()) speed = GROUND_SPEED;
	else						  speed = AIR_SPEED;
	if (m_pKbTracker->GetLastState().S)
	{
		force += speed * -forward;
	}
	else if (m_pKbTracker->GetLastState().W)
	{
		force -= speed * -forward;
	}
	if (m_pKbTracker->GetLastState().D)
	{
		force += speed * right;
	}
	else if (m_pKbTracker->GetLastState().A)
	{
		force -= speed * right;
	}

	//回転
	if (force.x != 0.0f || force.z != 0.0f)
	{
		m_pPlayer->SetRotY(std::atan2f(-force.x, -force.z));
	}

	// 座標の更新
	m_pPlayer->GetPhysics()->CalculateVelocity(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime);
	m_pPlayer->GetPhysics()->AddFliction(m_pPlayer->GetVelocity(), m_pPlayer->GetOnGround());
	m_pPlayer->SetVelocity(m_pPlayer->GetVelocity() + force * elapsedTime);
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// 待機状態に切り替え
	if (!m_pKbTracker->GetLastState().W && !m_pKbTracker->GetLastState().S && !m_pKbTracker->GetLastState().A && !m_pKbTracker->GetLastState().D)
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Idle());
	}
}


void Player_Walk::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection);
}


void Player_Walk::Finalize()
{

}
