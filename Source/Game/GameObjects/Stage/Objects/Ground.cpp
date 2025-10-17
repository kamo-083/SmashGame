/**
 * @file   Ground.cpp
 *
 * @brief  地面に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Ground.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Ground::Ground(ID3D11DeviceContext* context)
	: m_position{ DirectX::SimpleMath::Vector3::Zero }
	, m_halfLength{ DirectX::SimpleMath::Vector3::Zero }
	, m_angle{ DirectX::SimpleMath::Vector3::Zero }
	, m_collider{}
	, m_collisionHandle{ 0 }
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
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
 * @param なし
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

	// 当たり判定の作成
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(m_halfLength);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotY, rotX, rotZ));

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
 * @param なし
 *
 * @return なし
 */
void Ground::Update()
{

}



/**
 * @brief 描画処理
 *
 * @param なし
 *
 * @return なし
 */
void Ground::Draw(RenderContext& context)
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

	m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::GreenYellow);
}



/**
 * @brief 終了処理
 *
 * @param なし
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
 * @param なし
 *
 * @return OBBのコライダー
 */

OBBCollider Ground::GetCollider()
{
	return m_collider;
}
