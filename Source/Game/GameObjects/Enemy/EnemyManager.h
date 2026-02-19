/**
 * @file   EnemyManager.h
 *
 * @brief  敵マネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/Effect/EffectManager.h"
#include "Source/Game/Data/EnemyData.h"
#include "Source/Game/Data/EnemyInfoLoader.h"
#include "Source/Game/GameObjects/Enemy/IEnemy.h"
#include "Source/Game/GameObjects/Enemy/GroundEnemy/GroundEnemy.h"


// クラスの定義 ===============================================================
class Player;
class StageScene;


// クラスの定義 ===============================================================
/**
 * @brief 敵マネージャー
 */
class EnemyManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 出現時の設定データ
	struct SpawnData
	{
		EnemyType type = EnemyType::Basic;		// 種類
		DirectX::SimpleMath::Vector3 position;	// 座標
		bool dropRespawn = false;				// 落下時リスポーンの有無
	};

	struct EnemyData
	{
		uint32_t id;					// 管理ID
		EnemyType type;					// 種類
		bool alive;						// 生存フラグ
		std::unique_ptr<IEnemy> enemy;	// 敵のポインタ

		EnemyData(uint32_t inId, EnemyType inType, std::unique_ptr<IEnemy> inEnemy)
			:id(inId)
			, type(inType)
			, alive(true)
			, enemy(std::move(inEnemy))
		{
		}
	};

	static constexpr float KILL_HEIGHT = -50.0f;	// 落下判定する高さ

	// データメンバの宣言 -----------------------------------------------
private:
	// シーンへのポインタ
	StageScene* m_pScene;

	// ユーザーリソースのポインタ
	UserResources* m_pUserResources;

	// 当たり判定マネージャーのポインタ
	CollisionManager* m_pCollisionManager;

	// エフェクトマネージャーのポインタ
	EffectManager* m_pEffectManager;

	// ID
	uint32_t m_nextID;

	// 敵の配列
	std::vector<std::unique_ptr<EnemyData>> m_enemies;

	// 種類ごとの情報
	std::vector<EnemyInfoLoader::EnemyInfo> m_enemyInfo;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyManager(
		UserResources* pUR,
		CollisionManager* pCM,
		EffectManager* pEM,
		StageScene* pScene);

	// デストラクタ
	~EnemyManager();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update(float elapsedTime, Player* pPlayer);

	// 描画処理
	void Draw(const RenderContext& context);

	// 終了処理
	void Finalize();

	// 出現
	EnemyData* Spawn(const SpawnData& spawnData);


// 取得/設定
public:	
	// IDから敵を取得
	IEnemy* GetEnemyByID(uint32_t id) const;

	// 敵の数を取得
	int GetEnemyNum() const { return static_cast<int>(m_enemies.size()); }


// 内部実装
private:
	// 敵の生成
	std::unique_ptr<IEnemy> Create(EnemyType type);

};
