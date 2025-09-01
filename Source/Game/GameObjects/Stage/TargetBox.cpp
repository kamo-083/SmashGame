/**
 * @file   TargetBox.cpp
 *
 * @brief  TargetBoxに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TargetBox.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TargetBox::TargetBox(ID3D11DeviceContext* context)
{
	m_box = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
TargetBox::~TargetBox()
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
void TargetBox::Initialize(CollisionManager* pCollisionManager,
						   Goal* goal,
						   DirectX::SimpleMath::Vector3 position,
						   DirectX::SimpleMath::Vector3 halfLength,
						   DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;
	m_velocity = SimpleMath::Vector3::Zero;

	m_pGoal = goal;

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
	m_collisionHandle = pCollisionManager->Add(desc);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TargetBox::Update(float elapsedTime)
{
	
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TargetBox::Draw(RenderContext& context)
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

	m_box->Draw(world, context.view, context.projection, DirectX::Colors::Violet);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TargetBox::Finalize()
{

}

bool TargetBox::DetectCollisionToEnemy(SphereCollider enemy, StateType state)
{
	bool hit = IsHit(m_collider, enemy);

	if (hit)
	{
		if (state == StateType::Bounce) m_pGoal->CanGoal();
	}

	return hit;
}
