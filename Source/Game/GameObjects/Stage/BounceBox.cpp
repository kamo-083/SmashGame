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
{
	m_box = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
BounceBox::~BounceBox()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void BounceBox::Initialize(DirectX::SimpleMath::Vector3 position,
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
	m_collider.SetAxes(m_angle);
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
void BounceBox::Draw(RenderContext& context)
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

bool BounceBox::DetectCollisionToAttack(SphereCollider sphere, float power)
{
	bool hit = IsHit(m_collider, sphere);

	if (hit)
	{
		MTV mtv = CalculateMTV(m_collider, sphere);

		// 吹っ飛ぶ方向の設定
		DirectX::SimpleMath::Vector3 knockbackDir = mtv.direction;
		knockbackDir.Normalize();

		// 吹っ飛ぶ力の設定
		float knockbackForce = mtv.distance * power;

		DirectX::SimpleMath::Vector3 force = knockbackDir * knockbackForce;
		m_physics.GetExternalForce().Add(force);
	}

	return hit;
}