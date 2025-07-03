/**
 * @file   Player.cpp
 *
 * @brief  プレイヤーに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Player.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Player::Player()
	:m_position{0.0f,0.0f,0.0f}
	,m_velocity{0.0f,0.0f,0.0f}
	,m_pResourceManager{nullptr}
	,m_resources{}
	,m_physics{}
	,m_pCamera{nullptr}
{

}



/**
 * @brief デストラクタ
 */
Player::~Player()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player::Initialize(ResourceManager* pResourceManager, Camera* pCamera)
{
	//座標の初期化
	m_position = SimpleMath::Vector3::Zero;

	//速度の初期化
	m_velocity = SimpleMath::Vector3::Zero;

	//リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	//モデルの読み込み
	m_resources.model = m_pResourceManager->RequestSDKMESH("player", L"Resources\\Models\\mouse.sdkmesh");

	//コライダーの設定
	m_collider = SphereCollider(m_position, RADIUS);

	//カメラの設定
	m_pCamera = pCamera;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player::Update(DirectX::Keyboard::State keyboard, float elapsedTime)
{
	DirectX::SimpleMath::Vector3 forward = m_pCamera->GetForward();
	DirectX::SimpleMath::Vector3 right = forward.Cross(m_pCamera->GetUp());
	DirectX::SimpleMath::Vector3 force = DirectX::SimpleMath::Vector3::Zero;

	//移動
	if (keyboard.Down)
	{
		force += SPEED * -forward;
	}
	else if (keyboard.Up)
	{
		force -= SPEED * -forward;
	}
	if (keyboard.Right)
	{
		force += SPEED * right;
	}
	else if (keyboard.Left)
	{
		force -= SPEED * right;
	}

	m_physics.CalculateVelocity(m_velocity, MASS, true, elapsedTime);
	m_velocity += force * elapsedTime;
	m_physics.AddFliction(m_velocity, 0.15f);
	m_position += m_velocity * elapsedTime;

	//コライダーの更新
	m_collider.SetCenter(m_position);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	DirectX::SimpleMath::Matrix world;
	world = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	m_resources.model->Draw(context.deviceContext, *context.states, 
						    world, context.view, context.projection);

	debugFont->AddString(0, 100, DirectX::Colors::White, L"pos = %f,%f,%f", m_position.x, m_position.y, m_position.z);
	debugFont->AddString(0, 125, DirectX::Colors::White, L"vel = %f,%f,%f", m_velocity.x, m_velocity.y, m_velocity.z);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Player::Finalize()
{

}

bool Player::DetectCollisionToEnemy(SphereCollider collider)
{
	return IsHit(m_collider, collider);
}
