/**
 * @file   Player_Idle.cpp
 *
 * @brief  Player_Idleに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "Player_Idle.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player_Idle::Player_Idle(Player* Player, DirectX::Keyboard::KeyboardStateTracker* kbTracker)
	: m_pPlayer{ Player }
	, m_pKbTracker{ kbTracker }
{

}


/**
 * @brief デストラクタ
 */
Player_Idle::~Player_Idle()
{

}


void Player_Idle::Initialize(ResourceManager* pResourceManager)
{
	m_model = pResourceManager->RequestSDKMESH("player", L"Resources/Models/mouse.sdkmesh");
}


void Player_Idle::Update(const float& elapsedTime)
{
	// 座標の更新
	m_pPlayer->GetPhysics()->CalculateForce(m_pPlayer->GetVelocity(), m_pPlayer->GetMass(), elapsedTime, m_pPlayer->GetOnGround());
	m_pPlayer->LimitVelocity(m_pPlayer->GetVelocity());
	m_pPlayer->SetPosition(m_pPlayer->GetPosition() + m_pPlayer->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pPlayer->GetCollider()->SetCenter(m_pPlayer->GetPosition());

	m_pPlayer->SetOnGround(false);

	// 武器の切り替え
	m_pPlayer->ChangeWeapon(m_pKbTracker);

	// 歩き状態に切り替え
	if (m_pKbTracker->GetLastState().W || m_pKbTracker->GetLastState().S || m_pKbTracker->GetLastState().A || m_pKbTracker->GetLastState().D)
	{
		m_pPlayer->ChangeState(m_pPlayer->GetState_Walk());
	}

	// 攻撃状態に切り替え
	if (m_pKbTracker->pressed.Space)
	{
		m_pPlayer->Attack();
	}
}


void Player_Idle::Render(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_pPlayer->GetPosition());
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationY(m_pPlayer->GetRotY());
	world = rot * trans;

	m_model->Draw(context.deviceContext, *context.states, world, context.view, context.projection);
}


void Player_Idle::Finalize()
{

}
