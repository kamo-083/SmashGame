/**
 * @file   StageManager.h
 *
 * @brief  ステージマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include <functional>
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Data/StageLoader.h"


// クラスの宣言	===============================================================
class DebugFont;
class StageScene;
class UserResources;
class CollisionManager;
class EffectManager;
class EnemyManager;
class Ground;
class TargetBox;
class Goal;
class CountArea;
class Fence;
class Key;


// クラスの定義 ===============================================================
/**
 * @brief ステージマネージャー
 */
class StageManager
{
	// クラス定数の宣言 -------------------------------------------------
public:


	// データメンバの宣言 -----------------------------------------------
private:
	// シーンへのポインタ
	StageScene* m_pScene;

	// 深度ステンシルステート(各ステージオブジェクトに渡すため保持)
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	// 地面
	std::vector<std::unique_ptr<Ground>> m_grounds;

	// 的
	std::vector<std::unique_ptr<TargetBox>> m_targetBoxes;

	// エリア
	std::vector<std::unique_ptr<CountArea>> m_areas;

	// 柵
	std::vector<std::unique_ptr<Fence>> m_fences;

	// ゴール
	std::unique_ptr<Goal> m_goal;

	// 鍵
	std::unique_ptr<Key> m_key;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageManager(
		StageScene* pScene,
		ID3D11DepthStencilState* pDepthStencilState);

	// デストラクタ
	~StageManager();


// 操作
public:
	// ステージを生成
	void CreateStage(
		UserResources* pUR, CollisionManager* pCM, EnemyManager* pEnM, EffectManager* pEfM,
		const std::string& path);

	// 更新処理
	void Update(float elapsedTime, const DirectX::SimpleMath::Vector3& cameraPos, const DirectX::SimpleMath::Vector3& cameraUp);

	// 描画処理
	void Draw(const RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();

	// 操作の作成
	void CreateOperate(
		std::function<void()>& outOperate,
		StageLoader::AreaActionDesc& desc,
		const DirectX::SimpleMath::Vector3& position);


// 取得/設定
public:
	// ゴールしているか
	bool IsGoal() const;
	// ゴールできるか
	bool IsCanGoal() const;

	// 指定した地面の取得
	Ground* GetGround(const int groundNum) { return m_grounds[groundNum].get(); }

// 内部実装
private:

};
