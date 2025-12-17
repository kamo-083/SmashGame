/**
 * @file   Scene.cpp
 *
 * @brief  シーンの基底クラスのソースファイル	
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Common/SceneManager.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSceneManager  シーンマネージャーのポインタ
 * @param pUserResources ユーザーリソースのポインタ
 */
Scene::Scene(SceneManager* pSceneManager, UserResources* pUserResources)
	: m_sceneManager{ pSceneManager }
	, m_userResources{ pUserResources }
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
 * @brief シーン変更
 * 
 * @param nextSceneName 変更先のシーン名
 * 
 * @return なし
 */
void Scene::ChangeScene(const std::string& nextSceneName)
{
	m_sceneManager->RequestSceneChange(nextSceneName);
}