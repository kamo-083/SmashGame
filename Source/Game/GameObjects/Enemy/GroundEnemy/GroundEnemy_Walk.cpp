/**
 * @file   GroundEnemy_Walk.cpp
 *
 * @brief  GroundEnemy_Walkに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ==================================================
#include "pch.h"
#include "GroundEnemy_Walk.h"
#include "Source/Game/GameObjects/Enemy/IEnemy.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] groundEnemy 敵のポインタ
 */
GroundEnemy_Walk::GroundEnemy_Walk(GroundEnemy* groundEnemy)
	: m_pGroundEnemy{ groundEnemy }
	, m_stateType{ StateType::Walk }
{

}


/**
 * @brief デストラクタ
 */
GroundEnemy_Walk::~GroundEnemy_Walk()
{

}


/**
 * @brief 初期化処理
 *
 * @param[in] pRM  リソースマネージャーのポインタ
 *
 * @return なし
 */
void GroundEnemy_Walk::Initialize(ResourceManager* pRM)
{
	if (!m_modelAnimator)
	{
		m_modelAnimator = std::make_unique<ModelAnimator>(
			m_pGroundEnemy->GetModel(),
			m_pGroundEnemy->GetAnimation()->walk
		);
	}
	m_modelAnimator->Initialize();
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void GroundEnemy_Walk::Update(const float& elapsedTime)
{
	// プレイヤーとの距離と方向を取得
	IEnemy::PlayerRelationData playerData = m_pGroundEnemy->GetPlayerRelativeData();

	DirectX::SimpleMath::Vector3 force = playerData.direction * MOVE_SPEED * elapsedTime;

	//回転
	if (playerData.direction.x != 0.0f || playerData.direction.z != 0.0f)
	{
		m_pGroundEnemy->SetRotY(std::atan2f(-playerData.direction.x, -playerData.direction.z));
	}

	// 座標の更新
	m_pGroundEnemy->SetVelocity(m_pGroundEnemy->GetVelocity() + force);
	m_pGroundEnemy->GetPhysics()->CalculateForce(m_pGroundEnemy->GetVelocity(), m_pGroundEnemy->GetMass(), elapsedTime, m_pGroundEnemy->GetOnGround());
	m_pGroundEnemy->SetPosition(m_pGroundEnemy->GetPosition() + m_pGroundEnemy->GetVelocity() * elapsedTime);

	// 当たり判定の更新
	m_pGroundEnemy->GetCollider()->SetCenter(m_pGroundEnemy->GetPosition());

	m_pGroundEnemy->SetOnGround(false);

	// アニメーションの更新
	m_modelAnimator->Update(elapsedTime);

	// 攻撃状態に切り替え
	if (playerData.distance <= m_pGroundEnemy->GetAttackCollider()->GetRadius())
	{
		m_pGroundEnemy->SetIsAttack(true);
		m_pGroundEnemy->SetAttackCollisionEnabled(true);
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Attack());
	}

	// 待機状態に切り替え
	if (playerData.distance > GroundEnemy::DitectionRange)
	{
		m_pGroundEnemy->ChangeState(m_pGroundEnemy->GetState_Idle());
	}
}


/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 *
 * @return なし
 */
void GroundEnemy_Walk::Render(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_pGroundEnemy->GetPosition());
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_pGroundEnemy->GetRotY());
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_pGroundEnemy->GetScale());
	world = scale * rot * trans;

	m_modelAnimator->Draw(context, world);
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GroundEnemy_Walk::Finalize()
{

}
