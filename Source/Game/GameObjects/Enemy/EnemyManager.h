/**
 * @file   EnemyManager.h
 *
 * @brief  EnemyManagerに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/Effect/EffectManager.h"
#include "Source/Game/Data/EnamyData.h"
#include "Source/Game/GameObjects/Enemy/EnemyInfoLoader.h"
#include "Source/Game/GameObjects/Enemy/IEnemy.h"
#include "Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"


// クラスの定義 ===============================================================
class Player;


// クラスの定義 ===============================================================
/**
 * @brief EnemyManager
 */
class EnemyManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct SpawnData
	{
		EnemyType type;
		DirectX::SimpleMath::Vector3 position;
	};

	struct EnemyData
	{
		uint32_t id;
		EnemyType type;
		bool alive;
		std::unique_ptr<IEnemy> enemy;

		EnemyData(uint32_t inId, EnemyType inType, std::unique_ptr<IEnemy> inEnemy)
			:id(inId)
			, type(inType)
			, alive(true)
			, enemy(std::move(inEnemy))
		{
		}
	};

	static constexpr float KILL_HEIGHT = -50.0f;

	// データメンバの宣言 -----------------------------------------------
private:
	// ユーザーリソースのポインタ
	UserResources* m_pUserResources;

	// コリジョンマネージャーのポインタ
	CollisionManager* m_pCollisionManager;

	// エフェクトマネージャーのポインタ
	EffectManager* m_pEffectManager;

	// ID
	uint32_t m_nextID;

	// 敵
	std::vector<std::unique_ptr<EnemyData>> m_enemies;

	// 種類ごとの情報
	std::vector<EnemyInfoLoader::EnemyInfo> m_enemyInfo;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyManager(UserResources* pUserResources, 
				 CollisionManager* pCollisionManager,
				 EffectManager* pEffectManager);

	// デストラクタ
	~EnemyManager();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update(float elapsedTime, Player* pPlayer);

	// 描画処理
	void Draw(RenderContext& context);

	// 終了処理
	void Finalize();

	// 出現
	EnemyData* Spawn(const SpawnData& spawnData);

	// IDから敵を取得
	IEnemy* GetEnemyByID(uint32_t id) const;


// 取得/設定
public:

// 内部実装
private:
	std::unique_ptr<IEnemy> Create(EnemyType type);

};
