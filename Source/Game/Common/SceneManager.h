/**
 * @file   SceneManager.h
 *
 * @brief  シーンの管理クラスのヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <memory>
#include <string>
#include <unordered_map>
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Common/ResourceManager.h"

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

	std::unique_ptr<ResourceManager> m_resourceManager;

	DX::DeviceResources* m_pDeviceResources;

	Imase::DebugFont m_debugFont;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	SceneManager(DX::DeviceResources* pDR);

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
	ResourceManager* GetResourceManager() { return m_resourceManager.get(); }
	DX::DeviceResources* GetDeviceResources() { return m_pDeviceResources; }

// 内部実装
private:
	// シーンの変更
	void ChangeScene();
};
