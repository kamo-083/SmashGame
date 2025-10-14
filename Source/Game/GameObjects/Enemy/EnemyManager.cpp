/**
 * @file   EnemyManager.cpp
 *
 * @brief  EnemyManagerに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyManager.h"
#include "Source/Game/GameObjects/Player/Player.h"


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
	EnemyInfoLoader loader;
	loader.LoadData("Resources/Json/enemyInfo.json", m_enemyInfo);
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
	for (auto& e : m_enemies)
	{
		e->enemy->Update(elapsedTime);

		e->enemy->CalculatePlayerRelationData(pPlayer->GetPosition(), pPlayer->GetRadius());

		// 落下した敵の活動を停止
		if (e->enemy->GetPosition().y < KILL_HEIGHT) e->alive = false;
	}

	// 生存中でない敵を削除
	m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
		[&](std::unique_ptr<EnemyData>& e)
		{
			if (!e->alive)
			{
				e->enemy->Finalize(m_pCollisionManager);
				return true;
			}
			return false;
		}),
		m_enemies.end());
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
	for (auto& e : m_enemies)
	{
		e->enemy->Draw(context, m_pUserResources->GetDebugFont());
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
	for (auto& e : m_enemies)
	{
		e->enemy->Finalize(m_pCollisionManager);
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
	std::unique_ptr<IEnemy> enemy = Create(spawnData.type);
	enemy->Initialize(m_pUserResources->GetResourceManager(),
					  m_pCollisionManager,
					  spawnData.position,
					  m_enemyInfo[static_cast<int>(spawnData.type)],
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
IEnemy* EnemyManager::GetEnemyByID(uint32_t id) const
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
std::unique_ptr<IEnemy> EnemyManager::Create(EnemyType type)
{
	return std::make_unique<GroundEnemy>(
		m_enemyInfo[static_cast<int>(type)],
		m_pUserResources, m_pEffectManager);

	return nullptr;
}
