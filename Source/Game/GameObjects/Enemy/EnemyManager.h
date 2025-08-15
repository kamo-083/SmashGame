/**
 * @file   EnemyManager.h
 *
 * @brief  EnemyManagerに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/GameObjects/Enemy/Enemy.h"
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
	enum EnemyType
	{
		Ground,
	};

	struct SpawnData
	{
		EnemyType type;
		DirectX::SimpleMath::Vector3 position;
	};

	struct EnemyData
	{
		int id;
		EnemyType type;
		bool alive;
		std::unique_ptr<Enemy> enemy;

		EnemyData(int inId, EnemyType inType, std::unique_ptr<Enemy> inEnemy)
			:id(inId)
			, type(inType)
			, alive(true)
			, enemy(std::move(inEnemy))
		{
		}
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// ユーザーリソースのポインタ
	UserResources* m_pUserResources;

	// ID
	int m_nextID;

	// 敵
	std::vector<std::unique_ptr<EnemyData>> m_enemies;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	EnemyManager(UserResources* pUserResources);

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

// 取得/設定
public:

// 内部実装
private:
	std::unique_ptr<Enemy> Create(EnemyType type);

};
