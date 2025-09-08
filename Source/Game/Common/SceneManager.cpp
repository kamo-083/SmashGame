/**
 * @file   SceneManager.cpp
 *
 * @brief  シーンの管理クラスのソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/Scene.h"


using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
SceneManager::SceneManager(UserResources* pUserResources)
	: m_scenes{}
	, m_pCurrentScene{ nullptr }
	, m_pRequestedScene{ nullptr }
	, m_sharedData{}
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
 * @param[in] sceneName シーンの登録名
 * @param[in] scene     登録シーン
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
 * @param[in] inputDevice 入力デバイス
 * @param[in] elapsedTime フレーム経過時間
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
 * @param[in] なし
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

	//シーン名ここで描画してもいいかもしれない
	m_userResources->GetDebugFont()->AddString(0, 0, Colors::White, L"SceneManager");
}


	
/**
 * @brief 開始シーンの設定
 *
 * @param[in] startSceneName 開始シーン名
 *
 * @return なし
 */
void SceneManager::SetStartScene(const std::string& startSceneName)
{
	RequestSceneChange(startSceneName);

	//if (m_scenes.count(startSceneName) == 0)
	//{
	//	GameLib::ShowErrorMessage("%sシーンは登録されていません。", startSceneName.c_str());
	//	return;
	//}

	m_pCurrentScene = m_scenes[startSceneName].get();
	//m_pCurrentScene->Initialize();
}



/**
 * @brief シーン変更の要求
 *
 * @param[in] requestSceneName 変更予定のシーン名
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
 * @param[in] なし
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
		m_pCurrentScene->Finalize();
	}

	m_pCurrentScene = m_pRequestedScene;
	m_pCurrentScene->Initialize();

	m_pRequestedScene = nullptr;
}

