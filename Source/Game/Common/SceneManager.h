/**
 * @file   SceneManager.h
 *
 * @brief  シーンの管理クラスのヘッダファイル
 *
 * @author 清水まこと
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

#include "ImaseLib/DebugFont.h"


// クラスの宣言 ===============================================================
class Scene;




// クラスの定義 ===============================================================
/**
 * @brief シーン管理
 */
class SceneManager
{
// エイリアス宣言 ---------------------------------------------------
private:
	using SceneCollection = std::unordered_map<std::string, std::unique_ptr<Scene>>;
	using SharedData      = std::unordered_map<std::string, std::string>;



// データメンバの宣言 -----------------------------------------------
private:
	SceneCollection m_scenes;    ///< シーン群

	Scene* m_pCurrentScene;      ///< 現在のシーン
	Scene* m_pRequestedScene;    ///< 変更要求のあったシーン

	SharedData m_sharedData;    ///< 共有データ

	UserResources* m_userResources;


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
	void Render(RenderContext context);

	// 開始シーンの設定
	void SetStartScene(const std::string& startSceneName);

	// シーン変更の要求
	void RequestSceneChange(const std::string& nextSceneName);


// 取得/設定
public:
	UserResources* GetUserResources() { return m_userResources; }

// 内部実装
private:
	// シーンの変更
	void ChangeScene();
};
