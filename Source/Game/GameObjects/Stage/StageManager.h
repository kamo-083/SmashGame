/**
 * @file   StageManager.h
 *
 * @brief  StageManagerに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Game/Common/RenderContext.h"


// クラスの宣言	===============================================================
class Ground;
class BounceBox;
class TargetBox;
class Goal;
class CountArea;


// クラスの定義 ===============================================================
/**
 * @brief StageManager
 */
class StageManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	enum class ObjectType
	{
		Ground,
		BounceBox,
		TargetBox,
		Area,
		Goal
	};

	struct ObjectData
	{
		ObjectType type;
		DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
		DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;
		bool active = true;
	};


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
	void CreateStage(const UserResources* pUR, const CollisionManager* pCM);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context, Imase::DebugFont* debugFont);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
