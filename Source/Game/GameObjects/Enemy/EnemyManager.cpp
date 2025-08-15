/**
 * @file   EnemyManager.cpp
 *
 * @brief  EnemyManagerに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyManager.h"
#include "Source/Game/GameObjects/Player/Player.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EnemyManager::EnemyManager(UserResources* pUserResources)
	:m_pUserResources{pUserResources}
	,m_nextID{0}
{

}



/**
 * @brief デストラクタ
 */
EnemyManager::~EnemyManager()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Initialize()
{

}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Update(float elapsedTime, Player* pPlayer)
{
	for (auto& enemy : m_enemies)
	{
		if (!enemy->alive) continue;

		enemy->enemy->Update(elapsedTime);

		enemy->enemy->CalculatePlayerRelationData(pPlayer->GetPosition(), pPlayer->GetRadius());
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Draw(RenderContext& context)
{
	for (auto& enemy : m_enemies)
	{
		enemy->enemy->Draw(context, m_pUserResources->GetDebugFont());
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyManager::Finalize()
{

}

EnemyManager::EnemyData* EnemyManager::Spawn(const SpawnData& spawnData)
{
	// IDを更新
	m_nextID++;

	// 敵を生成
	std::unique_ptr<Enemy> enemy = Create(spawnData.type);
	enemy->Initialize(m_pUserResources->GetResourceManager(), spawnData.position);

	// 配列に登録
	std::unique_ptr<EnemyData> data = std::make_unique<EnemyData>
		(
			m_nextID,
			spawnData.type,
			std::move(enemy)
		);
	m_enemies.push_back(std::move(data));

	return m_enemies.back().get();
}

std::unique_ptr<Enemy> EnemyManager::Create(EnemyType type)
{
	switch (type)
	{
	case Ground:
		return std::make_unique<GroundEnemy>(m_pUserResources);
	}

	return nullptr;
}
