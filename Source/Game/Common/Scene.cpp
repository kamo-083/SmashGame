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
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/UserResources.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Scene::Scene(SceneManager* pSceneManager, UserResources* pUserReources)
	: m_sceneManager{ pSceneManager }
	, m_userResorces{ pUserReources }
	, m_isSceneActive{ false }
{

}



/**
 * @brief デストラクタ
 */
Scene::~Scene()
{
	m_sceneManager = nullptr;
	m_userResorces = nullptr;

	m_kbTracker.Reset();
}


/**
 * @brief シーンの変更
 *
 * @param[in] nextSceneName 変更先のシーン名
 *
 * @return なし
 */
void Scene::ChangeScene(const std::string& nextSceneName)
{
	m_sceneManager->RequestSceneChange(nextSceneName);
}


bool Scene::IsSceneActive()
{
	return m_isSceneActive;
}