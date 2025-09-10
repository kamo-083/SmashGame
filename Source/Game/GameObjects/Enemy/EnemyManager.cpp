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
 * @param[in] pUserResources	ユーザーリソースのポインタ
 * @param[in] pCollisionManager コリジョンマネージャーのポインタ
 * @param[in] pEffectManager エフェクトマネージャーのポインタ
 */
EnemyManager::EnemyManager(UserResources* pUserResources,
						   CollisionManager* pCollisionManager,
						   EffectManager* pEffectManager)
	:m_pUserResources{pUserResources}
	,m_pCollisionManager{pCollisionManager}
	,m_pEffectManager{pEffectManager}
	,m_nextID{0}
{

}



/**
 * @brief デストラクタ
 */
EnemyManager::~EnemyManager()
{
	m_enemies.clear();
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
 * @param[in] elapsedTime 経過時間
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
 * @param[in] context	描画用構造体
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
	for (auto& enemy : m_enemies)
	{
		enemy->enemy->Finalize(m_pCollisionManager);
	}
	m_enemies.clear();
}


/**
 * @brief 敵を出現させる
 *
 * @param[in] spawnData 出現させる敵の情報
 *
 * @return 出現した敵の情報
 */
EnemyManager::EnemyData* EnemyManager::Spawn(const SpawnData& spawnData)
{
	// IDを更新
	m_nextID++;

	// 敵を生成
	std::unique_ptr<Enemy> enemy = Create(spawnData.type);
	enemy->Initialize(m_pUserResources->GetResourceManager(),
					  m_pCollisionManager,
					  spawnData.position,
					  m_nextID);

	// 配列に登録
	std::unique_ptr<EnemyData> enemyData = std::make_unique<EnemyData>
		(
			m_nextID,
			spawnData.type,
			std::move(enemy)
		);
	m_enemies.push_back(std::move(enemyData));

	return m_enemies.back().get();
}


/**
 * @brief IDから敵を取得
 *
 * @param[in] id 敵ID
 *
 * @return 敵のポインタ
 */
Enemy* EnemyManager::GetEnemyByID(uint32_t id) const
{
	// IDが一致する敵を探して返す
	for (auto& enemy : m_enemies)
	{
		if (enemy->alive && enemy->id == id) return enemy->enemy.get();
	}

	return nullptr;
}


/**
 * @brief 敵を生成
 *
 * @param[in] type 敵の種類
 *
 * @return 敵のユニークポインタ
 */
std::unique_ptr<Enemy> EnemyManager::Create(EnemyType type)
{
	switch (type)
	{
	case EnemyType::Basic:
		return std::make_unique<GroundEnemy>(
			BasicDesc.radius, BasicDesc.speed, BasicDesc.mass, BasicDesc.max_speed,
			m_pUserResources, m_pEffectManager);
	case EnemyType::Heavy:
		return std::make_unique<GroundEnemy>(
			HeavyDesc.radius, HeavyDesc.speed, HeavyDesc.mass, HeavyDesc.max_speed,
			m_pUserResources, m_pEffectManager);
	case EnemyType::Light:
		return std::make_unique<GroundEnemy>(
			LightDesc.radius, LightDesc.speed, LightDesc.mass, LightDesc.max_speed,
			m_pUserResources, m_pEffectManager);
	}

	return nullptr;
}
