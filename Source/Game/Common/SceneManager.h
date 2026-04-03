/**
 * @file   SceneManager.h
 *
 * @brief  シーンの管理クラスのヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <string>
#include <unordered_map>
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Debug/DebugFont.h"

// クラスの宣言 ===============================================================
class Scene;
class BlockTransition;
class UITextureCatalog;

// クラスの定義 ===============================================================
/**
 * @brief シーン管理
 */
class SceneManager
{
// クラス定数の宣言 ---------------------------------------------------
private:
	// シーン配列
	using SceneCollection = std::unordered_map<std::string, std::unique_ptr<Scene>>;

	static constexpr float TRANSITION_INTERVAL = 0.25f;

// データメンバの宣言 -----------------------------------------------
private:
	// シーン群
	SceneCollection m_scenes;    

	// 現在のシーン
	Scene* m_pCurrentScene;      

	// 変更要求のあったシーン
	Scene* m_pRequestedScene;    

	// ユーザーリソースのポインタ
	UserResources* m_userResources;	

	// UI使用画像のカタログ
	std::shared_ptr<UITextureCatalog> m_UITextureCatalog;

	// シーン遷移演出
	std::unique_ptr<BlockTransition> m_transition;

	// シーン間の共有データ
	std::unordered_map<std::string, std::string> m_shaerdData;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SceneManager(UserResources* pUserResources);

	// デストラクタ
	~SceneManager();

// 操作
public:
	// シーンの登録
	void Register(const std::string& sceneName, std::unique_ptr<Scene> scene);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(RenderContext& context);

	// 開始シーンの設定
	void SetStartScene(const std::string& startSceneName);

	// シーン変更の要求
	void RequestSceneChange(const std::string& nextSceneName);

// 取得/設定
public:
	// ユーザーリソースのポインタの取得
	UserResources* GetUserResources() { return m_userResources; }

	// シーン遷移演出の取得
	BlockTransition* GetTransition() { return m_transition.get(); }

	// UI使用画像のカタログの取得
	std::shared_ptr<UITextureCatalog>& GetUITextureCatalog() { return m_UITextureCatalog; }

	// 共有データの登録
	void SetSharedData(const std::string& name, const std::string& data);

	// 共有データの取得
	std::string GetSharedData(const std::string& name);

// 内部実装
private:
	// シーンの変更
	void ChangeScene();
};