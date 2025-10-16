/**
 * @file   StageManager.h
 *
 * @brief  StageManagerに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include <functional>
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/GameObjects/Stage/StageLoader.h"


// クラスの宣言	===============================================================
class Ground;
class BounceBox;
class TargetBox;
class Goal;
class CountArea;
class Fence;


// クラスの定義 ===============================================================
/**
 * @brief StageManager
 */
class StageManager
{
	// クラス定数の宣言 -------------------------------------------------
public:


	// データメンバの宣言 -----------------------------------------------
private:
	//地面
	std::vector<std::unique_ptr<Ground>> m_grounds;

	//箱
	std::vector<std::unique_ptr<BounceBox>> m_bounceBoxes;

	//的
	std::vector<std::unique_ptr<TargetBox>> m_targetBoxes;

	//エリア
	std::vector<std::unique_ptr<CountArea>> m_areas;

	//柵
	std::vector<std::unique_ptr<Fence>> m_fences;

	//ゴール
	std::unique_ptr<Goal> m_goal;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageManager();

	// デストラクタ
	~StageManager();


// 操作
public:
	// ステージを生成
	void CreateStage(UserResources* pUR, CollisionManager* pCM, EnemyManager* pEM,
					 const std::string& path);

	// 更新処理
	void Update(float elapsedTime, DirectX::SimpleMath::Vector3 cameraPos, DirectX::SimpleMath::Vector3 cameraUp);

	// 描画処理
	void Draw(RenderContext context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 操作の作成
	void CreateOperate(std::function<void()>& outOperate, StageLoader::AreaActionDesc& desc);


// 取得/設定
public:
	// ゴールしているか
	bool IsGoal();

// 内部実装
private:

};
