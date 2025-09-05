/**
 * @file   BounceBox.cpp
 *
 * @brief  BounceBoxに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BounceBox.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
BounceBox::BounceBox(ID3D11DeviceContext* context)
	: m_onGround{ false }
	, m_collisionHandle{ 0 }
{
	m_box = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
BounceBox::~BounceBox()
{
	m_box.reset();
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BounceBox::Initialize(CollisionManager* pCollisionManager,
						   DirectX::SimpleMath::Vector3 position,
						   DirectX::SimpleMath::Vector3 halfLength,
						   DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;
	m_velocity = SimpleMath::Vector3::Zero;
	m_onGround = true;

	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(m_halfLength);
	m_collider.SetRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(m_angle.y, m_angle.x, m_angle.z));

	// コリジョンマネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.callback.onEnter =
		[this, pCollisionManager](uint32_t, uint32_t other)
		{
			if (pCollisionManager->GetDesc(other)->layer != CollisionManager::Layer::PlayerAttack) return;

			MTV mtv = CalculateMTV(m_collider, *pCollisionManager->GetDesc(other)->sphere);

			// 吹っ飛ぶ方向の設定
			DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
			knockbackDir.Normalize();

			// 吹っ飛ぶ力の設定
			float knockbackForce = mtv.distance * *pCollisionManager->GetDesc(other)->uerData;

			DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
			m_physics.GetExternalForce().Add(force);

		};
	m_collisionHandle = pCollisionManager->Add(desc);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BounceBox::Update(float elapsedTime)
{
	// 座標の更新
	m_physics.CalculateForce(m_velocity, MASS, elapsedTime, m_onGround);
	m_position += m_velocity * elapsedTime;

	m_collider.SetCenter(m_position);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BounceBox::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_halfLength * 2.0f);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationX(rotX) *
									  DirectX::SimpleMath::Matrix::CreateRotationY(rotY) *
									  DirectX::SimpleMath::Matrix::CreateRotationZ(rotZ);
	world = scale * rot * trans;

	m_box->Draw(world, context.view, context.projection, DirectX::Colors::Yellow);

	debugFont->AddString(0, 350, DirectX::Colors::Yellow, L"ExternalForce = %f,%f,%f",
						 m_physics.GetExternalForce().Get().x, m_physics.GetExternalForce().Get().y, m_physics.GetExternalForce().Get().z);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BounceBox::Finalize()
{

}

bool BounceBox::DetectCollisionToBox(OBBCollider obb)
{
	bool hit = IsHit(m_collider, obb);

	if (hit)
	{
		MTV mtv = CalculateMTV(m_collider, obb);

		m_position -= mtv.direction * mtv.distance;
		m_collider.SetCenter(m_position);
	}

	return hit;
}

bool BounceBox::DetectCollisionToAttack(SphereCollider attack, SphereCollider player, float power)
{
	bool hit = IsHit(m_collider, attack);

	if (hit)
	{
		MTV mtv = CalculateMTV(m_collider, player);

		// 吹っ飛ぶ方向の設定
		DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;

		// 吹っ飛ぶ力の設定
		float knockbackForce = mtv.distance * power;

		DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
		m_physics.GetExternalForce().Add(force);
	}

	return hit;
}