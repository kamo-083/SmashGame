/**
 * @file   Fence.cpp
 *
 * @brief  柵に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Fence.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/Common/ResourceManager.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param context	デバイスコンテキストのポインタ
 */
Fence::Fence(ID3D11DeviceContext* context)
	: m_position{ DirectX::SimpleMath::Vector3::Zero }
	, m_scale{ DirectX::SimpleMath::Vector3::Zero }
	, m_angle{ DirectX::SimpleMath::Vector3::Zero }
	, m_collider{}
	, m_collisionHandle{ 0 }
	, m_model{ nullptr }
	, m_num{ 0 }
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Fence::~Fence()
{
	m_geometricPrimitive.reset();
}



/**
 * @brief 初期化処理
 *
 * @param pRM		リソースマネージャーのポインタ
 * @param pCM		当たり判定マネージャーのポインタ
 * @param position	位置
 * @param scale		スケール
 * @param angle		角度
 *
 * @return なし
 */
void Fence::Initialize(
	ResourceManager* pRM,
	CollisionManager* pCM,
	int num,
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Vector3 scale,
	DirectX::SimpleMath::Vector3 angle)
{
	// 位置・スケール・回転・柵数を設定
	m_position = position;
	m_scale = scale;
	m_angle = angle;
	m_num = num;

	// 当たり判定の大きさを計算
	DirectX::SimpleMath::Vector3 colliderSize = {
		FENCE_SIZE.x * m_scale.x / 2.0f,
		FENCE_SIZE.y * m_scale.y / 2.0f,
		FENCE_SIZE.z * m_num * m_scale.z / 2.0f
	};

	// 当たり判定の作成
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(colliderSize);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotY, rotX, rotZ));

	// 当たり判定マネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.restitution = RESTITUTION;
	m_collisionHandle = pCM->Add(desc);

	// モデルの読み込み
	m_model = pRM->RequestSDKMESH("fence", "fence.sdkmesh");
}



/**
 * @brief 描画処理
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void Fence::Draw(RenderContext& context)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_scale);
	float rotX = DirectX::XMConvertToRadians(m_angle.x);
	float rotY = DirectX::XMConvertToRadians(m_angle.y);
	float rotZ = DirectX::XMConvertToRadians(m_angle.z);
	DirectX::SimpleMath::Matrix rot = 
		DirectX::SimpleMath::Matrix::CreateRotationX(rotX) *
		DirectX::SimpleMath::Matrix::CreateRotationY(rotY) *
		DirectX::SimpleMath::Matrix::CreateRotationZ(rotZ);
	DirectX::SimpleMath::Matrix trans;
	DirectX::SimpleMath::Matrix world;

	// モデルの描画
	// 1つあたりの間隔を計算
	float width = FENCE_SIZE.z * m_scale.z;
	// 中心への補正
	float half = (m_num - 1) * 0.5f;
	// 描画
	for (int i = 0; i < m_num; i++)
	{
		// 表示位置を計算
		float t = (i - half) * width;
		DirectX::SimpleMath::Vector3 localOffset(0.0f, 0.0f, t);
		DirectX::SimpleMath::Vector3 worldOffset = 
			DirectX::SimpleMath::Vector3::Transform(localOffset, DirectX::SimpleMath::Matrix::CreateRotationY(rotY));
		DirectX::SimpleMath::Vector3 pos = m_position + worldOffset;

		trans = DirectX::SimpleMath::Matrix::CreateTranslation(pos);
		world = scale * rot * trans;
		m_model->Draw(context.deviceContext, *context.states, world, context.view, context.proj);
	}
	
	//// 当たり判定のデバッグ描画
	//trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	//scale = DirectX::SimpleMath::Matrix::CreateScale(m_collider.GetHalfLength() * 2.0f);
	//world = scale * rot * trans;
	//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Orange, nullptr, true);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void Fence::Finalize()
{
	m_model = nullptr;
	m_geometricPrimitive.reset();
}