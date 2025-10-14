/**
 * @file   SceneManager.cpp
 *
 * @brief  シーンの管理クラスのソースファイル
 *
 * @author 清水まこと	
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Common/SceneManager.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Scene::Scene(SceneManager* pSceneManager, UserResources* pUserResources)
	: m_sceneManager{ pSceneManager }
	, m_userResources{ pUserResources }
	, m_isSceneActive{ false }
{

}



/**
 * @brief デストラクタ
 */
Scene::~Scene()
{
	m_sceneManager = nullptr;
	m_userResources = nullptr;
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