/**
 * @file   Goal.cpp
 *
 * @brief  地面に関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Goal.h"
#include "Source/Game/Common/CollisionManager.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Goal::Goal(ID3D11DeviceContext* context)
	: m_position{ DirectX::SimpleMath::Vector3::Zero }
	, m_collider{}
	, m_collisionHandle{ 0 }
	, m_isGoal{ false }
	, m_canGoal{ false }
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
Goal::~Goal()
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
void Goal::Initialize(CollisionManager* pCollisionManager, DirectX::SimpleMath::Vector3 position)
{
	m_position = position;

	m_collider.SetCenter(m_position);
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::Identity);
	m_collider.SetHalfLength(DirectX::SimpleMath::Vector3(HALF_LENGTH, HALF_LENGTH, HALF_LENGTH));

	// コリジョンマネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Trigger;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.isTrigger = true;
	desc.callback.onEnter = 
		[this, pCollisionManager](uint32_t, uint32_t other)
		{
			if (!m_canGoal || pCollisionManager->GetDesc(other)->layer != CollisionManager::Layer::PlayerBody) return;

			m_isGoal = true;
		};
	m_collisionHandle = pCollisionManager->Add(desc);

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

	// 当たり判定のデバッグ描画
	m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Aqua, nullptr, true);

	// デバッグ情報の描画
	debugFont->AddString(0, 140, DirectX::Colors::Yellow, L" can = %d", m_canGoal);
	debugFont->AddString(100, 140, DirectX::Colors::Yellow, L"goal = %d", m_isGoal);
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
	m_geometricPrimitive.reset();
}


bool Goal::DetectCollisionToPlayer(SphereCollider player)
{
	if (!m_canGoal) return false;

	bool hit = IsHit(m_collider, player);
	if (hit)
	{
		// ゴールの判定処理
		DirectX::SimpleMath::Vector3 distance = m_position - player.GetCenter();
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
