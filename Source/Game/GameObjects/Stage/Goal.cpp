/**
 * @file   Goal.cpp
 *
 * @brief  地面に関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Goal.h"


using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Goal::Goal(ID3D11DeviceContext* context)
	:m_position{ SimpleMath::Vector3::Zero }
	, m_collider{}
	,m_isGoal{false}
	,m_canGoal{false}
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Goal::~Goal()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Goal::Initialize(DirectX::SimpleMath::Vector3 position)
{
	m_position = position;

	m_collider.SetCenter(m_position);
	m_collider.SetAxes(SimpleMath::Vector3::Zero);
	m_collider.SetHalfLength(SimpleMath::Vector3(HALF_LENGTH, HALF_LENGTH, HALF_LENGTH));

	m_canGoal = false;
	m_isGoal = false;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Goal::Update()
{

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Goal::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(HALF_LENGTH * 2.0f);
	world = scale * trans;

	m_geometricPrimitive->Draw(world, context.view, context.projection, DirectX::Colors::Aqua, nullptr, true);

	debugFont->AddString(0, 115, Colors::Yellow, L" can = %d", m_canGoal);
	debugFont->AddString(0, 145, Colors::Yellow, L"goal = %d", m_isGoal);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Goal::Finalize()
{

}


bool Goal::DetectCollisionToPlayer(SphereCollider player)
{
	if (!m_canGoal) return false;

	bool hit = IsHit(m_collider, player);
	if (hit)
	{
		// ゴールの判定処理
		SimpleMath::Vector3 distance = m_position - player.GetCenter();
		if (distance.Length() <= HALF_LENGTH) m_isGoal = true;
	}

	return hit;
}



/**
 * @brief コライダーの取得
 *
 * @param[in] なし
 *
 * @return OBBのコライダー
 */

OBBCollider Goal::GetCollider()
{
	return m_collider;
}
