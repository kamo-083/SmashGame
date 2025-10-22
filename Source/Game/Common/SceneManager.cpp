/**
 * @file   SceneManager.cpp
 *
 * @brief  シーンの管理クラスのソースファイル
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/Scene.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUserResources ユーザーリソースのポインタ
 */
SceneManager::SceneManager(UserResources* pUserResources)
	: m_scenes{}
	, m_pCurrentScene{ nullptr }
	, m_pRequestedScene{ nullptr }
	, m_userResources{ pUserResources }
{
}


/**
 * @brief デストラクタ
 */
SceneManager::~SceneManager()
{
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Finalize();
	}

	m_scenes.clear();
}


/**
 * @brief シーンの登録
 *
 * @param sceneName シーンの登録名
 * @param scene     登録シーン
 *
 * @return なし
 */
void SceneManager::Register(const std::string& sceneName, std::unique_ptr<Scene> scene)
{
	m_scenes.emplace(sceneName, std::move(scene));
}


/**
 * @brief 更新処理
 *
 * @param inputDevice 入力デバイス
 * @param elapsedTime フレーム経過時間
 *
 * @return なし
 */
void SceneManager::Update(float elapsedTime)
{
	// シーンの変更要求があった場合はシーンの変更を行う
	if (m_pRequestedScene)
	{
		ChangeScene();
	}


	// 現在のシーンを更新
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Update(elapsedTime);
	}
}


/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void SceneManager::Render(RenderContext context)
{
	// 現在のシーンを描画
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Render(context, m_userResources->GetDebugFont());
	}

	// デバッグ情報の追加
	m_userResources->GetDebugFont()->AddString(0, 0, DirectX::Colors::White, L"SceneManager");
}

	
/**
 * @brief 開始シーンの設定
 *
 * @param startSceneName 開始シーン名
 *
 * @return なし
 */
void SceneManager::SetStartScene(const std::string& startSceneName)
{
	RequestSceneChange(startSceneName);

	m_pCurrentScene = m_scenes[startSceneName].get();
}


/**
 * @brief シーン変更の要求
 *
 * @param requestSceneName 変更予定のシーン名
 *
 * @return なし
 */
void SceneManager::RequestSceneChange(const std::string& requestSceneName)
{
	if (m_scenes.count(requestSceneName) == 0)
	{
		return;
	}

	m_pRequestedScene = m_scenes[requestSceneName].get();
}


/**
 * @brief シーンの変更
 *
 * @param なし
 *
 * @return なし
 */
void SceneManager::ChangeScene()
{
	if (!m_pRequestedScene)
	{
		return;
	}

	if (m_pCurrentScene)
	{
		// 現在のシーンを終了
		m_pCurrentScene->Finalize();
	}

	// 次のシーンを設定
	m_pCurrentScene = m_pRequestedScene;
	m_pCurrentScene->Initialize();

	m_pRequestedScene = nullptr;
}