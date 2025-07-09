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



// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Ground::Ground(ID3D11DeviceContext* context)
	:m_position{CENTER_POS}
	,m_halfLength{HALF_LENGTH}
	,m_angle{ANGLE}
	,m_collider{}
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Ground::~Ground()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Ground::Initialize(DirectX::SimpleMath::Vector3 position,
						DirectX::SimpleMath::Vector3 halfLength,
						DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;

	m_collider.SetCenter(m_position);
	m_collider.SetAxes(m_angle);
	m_collider.SetHalfLength(m_halfLength);
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

	m_geometricPrimitive->Draw(world, context.view, context.projection, DirectX::Colors::GreenYellow);
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
