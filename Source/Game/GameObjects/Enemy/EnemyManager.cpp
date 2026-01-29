/**
 * @file   EnemyManager.cpp
 *
 * @brief  敵マネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyManager.h"
#include "Source/Game/GameObjects/Player/Player.h"
#include "Source/Game/Scenes/StageScene.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR		ユーザーリソースのポインタ
 * @param pCM		当たり判定マネージャーのポインタ
 * @param pEM		エフェクトマネージャーのポインタ
 * @param pScene	シーンのポインタ
 */
EnemyManager::EnemyManager(
	UserResources* pUR,
	CollisionManager* pCM,
	EffectManager* pEM,
	StageScene* pScene)
	:
	m_pScene{ pScene },
	m_pUserResources{ pUR },
	m_pCollisionManager{ pCM },
	m_pEffectManager{ pEM },
	m_nextID{ 0 }
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
 * @param なし
 *
 * @return なし
 */
void EnemyManager::Initialize()
{
	// データを読み込む
	EnemyInfoLoader loader;
	loader.LoadData("Resources/Json/enemyInfo.json", m_enemyInfo);
}


/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 * @param pPlayer	  プレイヤーのポインタ
 *
 * @return なし
 */
void EnemyManager::Update(float elapsedTime, Player* pPlayer)
{
	// 各敵の更新
	for (auto& e : m_enemies)
	{
		e->enemy->Update(elapsedTime);

		// プレイヤーとの位置関係を計算
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
				if (e->enemy->IsRespawn())
				{
					// リスポーン可能に設定されている場合
					e->enemy->Respawn();
					e->alive = true;
				}
				else
				{
					// そうでない場合は削除
					e->enemy->Finalize();
					return true;
				}
			}
			return false;
		}),
		m_enemies.end());
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void EnemyManager::Draw(const RenderContext& context)
{
	// 各敵の描画
	for (auto& e : m_enemies)
	{
		e->enemy->Draw(context, m_pUserResources->GetDebugFont());
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void EnemyManager::Finalize()
{
	for (auto& e : m_enemies)
	{
		e->enemy->Finalize();
	}
	m_enemies.clear();
}


/**
 * @brief 敵を出現させる
 *
 * @param spawnData 出現させる敵の情報
 *
 * @return 出現した敵の情報
 */
EnemyManager::EnemyData* EnemyManager::Spawn(const SpawnData& spawnData)
{
	// IDを更新
	m_nextID++;

	// 敵を生成
	std::unique_ptr<IEnemy> enemy = Create(spawnData.type);
	enemy->Initialize(
		m_pUserResources->GetResourceManager(),
		m_pCollisionManager,
		spawnData.position,
		m_enemyInfo[static_cast<int>(spawnData.type)],
		spawnData.dropRespawn,
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
 * @param id 敵ID
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
 * @param type 敵の種類
 *
 * @return 敵のユニークポインタ
 */
std::unique_ptr<IEnemy> EnemyManager::Create(EnemyType type)
{
	return std::make_unique<GroundEnemy>(
		m_enemyInfo[static_cast<int>(type)],
		m_pUserResources, m_pEffectManager,
		m_pScene);
}
