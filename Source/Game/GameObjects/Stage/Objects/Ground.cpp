/**
 * @file   Ground.cpp
 *
 * @brief  地面に関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Ground.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Ground::Ground(ID3D11DeviceContext* context)
	: m_position{ SimpleMath::Vector3::Zero }
	, m_halfLength{ SimpleMath::Vector3::Zero }
	, m_angle{ SimpleMath::Vector3::Zero }
	, m_collider{}
	, m_collisionHandle{ 0 }
{
	m_geometricPrimitive = GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Ground::~Ground()
{
	m_geometricPrimitive.reset();
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Ground::Initialize(CollisionManager* pCollisionManager,
						DirectX::SimpleMath::Vector3 position,
						DirectX::SimpleMath::Vector3 halfLength,
						DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;

	m_collider.SetCenter(m_position);
	m_collider.SetRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(m_angle.y, m_angle.x, m_angle.z));
	m_collider.SetHalfLength(m_halfLength);

	// コリジョンマネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.restitution = 0.0f;
	m_collisionHandle = pCollisionManager->Add(desc);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Ground::Update()
{

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Ground::Draw(RenderContext& context)
{
	SimpleMath::Matrix world;
	SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(m_position);
	SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(m_halfLength * 2.0f);
	float rotX = XMConvertToRadians(m_angle.x);
	float rotY = XMConvertToRadians(m_angle.y);
	float rotZ = XMConvertToRadians(m_angle.z);
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateRotationX(rotX) *
									  SimpleMath::Matrix::CreateRotationY(rotY) *
									  SimpleMath::Matrix::CreateRotationZ(rotZ);
	world = scale * rot * trans;

	m_geometricPrimitive->Draw(world, context.view, context.proj, Colors::GreenYellow);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Ground::Finalize()
{
	m_geometricPrimitive.reset();
}



/**
 * @brief コライダーの取得
 *
 * @param[in] なし
 *
 * @return OBBのコライダー
 */

OBBCollider Ground::GetCollider()
{
	return m_collider;
}
