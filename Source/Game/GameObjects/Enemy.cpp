/**
 * @file   Enemy.cpp
 *
 * @brief  敵に関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Enemy.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Enemy::Enemy()
	:m_position{0.0f,0.0f,0.0f}
	, m_pResourceManager{ nullptr }
	, m_resources{}
{

}



/**
 * @brief デストラクタ
 */
Enemy::~Enemy()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Enemy::Initialize(ResourceManager* pResourceManager)
{
	//座標の初期化
	m_position = { 0.0f,0.0f,0.0f };

	//リソースマネージャの設定
	m_pResourceManager = pResourceManager;

	//モデルの読み込み
	m_resources.model = m_pResourceManager->RequestSDKMESH("enemy", L"Resources\\Models\\player.sdkmesh");

	//コライダーの設定
	m_collider = SphereCollider(m_position, RADIUS);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Enemy::Update(DirectX::Keyboard::State keyboard, float elapsedTime)
{
	//コライダーの更新
	m_collider.SetCenter(m_position);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Enemy::Draw(RenderContext& context)
{
	DirectX::SimpleMath::Matrix world;
	world = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	m_resources.model->Draw(context.deviceContext, *context.states,
		world, context.view, context.projection);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Enemy::Finalize()
{

}

SphereCollider Enemy::GetCollider()
{
	return m_collider;
}
