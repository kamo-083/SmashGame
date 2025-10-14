/**
 * @file   TargetBox.cpp
 *
 * @brief  TargetBoxに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TargetBox.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TargetBox::TargetBox(ID3D11DeviceContext* context)
	: m_onGround{ false }
	, m_pGoal{ nullptr }
	, m_collisionHandle{ 0 }
{
	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(context, { 1.0f, 1.0f, 1.0f }, true);
}



/**
 * @brief デストラクタ
 */
TargetBox::~TargetBox()
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
void TargetBox::Initialize(CollisionManager* pCollisionManager,
						   EnemyManager* pEnemyManager,
						   Goal* goal,
						   DirectX::SimpleMath::Vector3 position,
						   DirectX::SimpleMath::Vector3 halfLength,
						   DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;
	m_velocity = DirectX::SimpleMath::Vector3::Zero;

	m_pGoal = goal;

	m_onGround = true;

	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(m_halfLength);
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_angle.y, m_angle.x, m_angle.z));

	// コリジョンマネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Stage;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.restitution = 0.0f;
	desc.callback.onEnter =
		[this, pCollisionManager, pEnemyManager](uint32_t, uint32_t other)
		{
			// 当たったのが敵本体なら通す
			if (pCollisionManager->GetDesc(other)->layer != CollisionManager::Layer::EnemyBody) return;

			//IDから敵を取得
			IEnemy* enemy = pEnemyManager->GetEnemyByID(pCollisionManager->GetDesc(other)->userId);

			// 当たった敵が跳ね返り状態ならゴール可能にする
			if (enemy->GetStateType() == StateType::Bounce)
			{
				m_pGoal->CanGoal();
			}
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

	m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Violet);
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
	m_geometricPrimitive.reset();
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
