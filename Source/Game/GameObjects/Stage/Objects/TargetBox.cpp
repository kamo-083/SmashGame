/**
 * @file   TargetBox.cpp
 *
 * @brief  的に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TargetBox.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param context	デバイスコンテキストのポインタ
 */
TargetBox::TargetBox(ID3D11DeviceContext* context)
	: m_pGoal{ nullptr }
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
 * @param pCM			当たり判定マネージャーのポインタ
 * @param pEM			敵マネージャーのポインタ
 * @param goal			ゴールのポインタ
 * @param position		位置
 * @param halfLength	大きさの半分
 * @param angle			回転角度
 *
 * @return なし
 */
void TargetBox::Initialize(CollisionManager* pCM,
						   EnemyManager* pEM,
						   Goal* goal,
						   DirectX::SimpleMath::Vector3 position,
						   DirectX::SimpleMath::Vector3 halfLength,
						   DirectX::SimpleMath::Vector3 angle)
{
	m_position = position;
	m_halfLength = halfLength;
	m_angle = angle;

	// ゴールのポインタを設定
	m_pGoal = goal;

	// 当たり判定の作成
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(m_halfLength);
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
	desc.restitution = 0.0f;
	desc.callback.onEnter =
		[this, pCM, pEM](uint32_t, uint32_t other)
		{
			// 当たったのが敵本体なら通す
			if (pCM->GetDesc(other)->layer != CollisionManager::Layer::EnemyBody) return;

			//IDから敵を取得
			IEnemy* enemy = pEM->GetEnemyByID(pCM->GetDesc(other)->userId);

			// 当たった敵が跳ね返り状態ならゴール可能にする
			if (enemy->GetStateType() == StateType::Bounce)
			{
				m_pGoal->CanGoal();
			}
		};
	m_collisionHandle = pCM->Add(desc);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
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

	m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Brown);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void TargetBox::Finalize()
{
	
}