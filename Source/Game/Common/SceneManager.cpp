/**
 * @file   SceneManager.cpp
 *
 * @brief  シーンの管理クラスのソースファイル
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Transition/BlockTransition.h"


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
	// シーン遷移演出の作成
	DirectX::SimpleMath::Vector2 windowSize ={ 
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().right),
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().bottom) };
	m_transition = std::make_unique<BlockTransition>(
		m_userResources->GetDeviceResources(), 
		m_userResources->GetShaderManager(),
		m_userResources->GetResourceManager(),
		ITransition::ResourcesDesc{ "blockVS","Shaders/Transition/TransitionVS.cso" },
		ITransition::ResourcesDesc{ "blockPS","Shaders/Transition/TransitionPS.cso" },
		ITransition::ResourcesDesc{ "blockGS","Shaders/Transition/TransitionGS.cso" },
		ITransition::ResourcesDesc{ "blockTransition", "Others/block_mask.png" },
		windowSize, TRANSITION_INTERVAL);
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
		m_transition->Update(elapsedTime);
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
		m_transition->Draw(context);
	}

	// デバッグ情報の追加
	m_userResources->GetDebugFont()->AddString(0, 0, DirectX::Colors::White, L"SceneManager");
	m_userResources->GetDebugFont()->AddString(180, 0, DirectX::Colors::Yellow, L"transition:%0.2f", m_transition->GetRate());
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