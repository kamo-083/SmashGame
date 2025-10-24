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


// クラスの定義 ===============================================================
/**
 * @brief シーン管理
 */
class SceneManager
{
// 定数の宣言 ---------------------------------------------------
private:
	// シーン配列
	using SceneCollection = std::unordered_map<std::string, std::unique_ptr<Scene>>;


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
	// ユーザーリソースのポインタの取得
	UserResources* GetUserResources() { return m_userResources; }

// 内部実装
private:
	// シーンの変更
	void ChangeScene();
};
