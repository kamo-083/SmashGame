/**
 * @file   StageSelectScene.cpp
 *
 * @brief  ＸＸＸＸシーンに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageSelectScene.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] sceneManager    シーンを管理しているマネージャ
 * @param[in] resourceManager リソースを管理しているマネージャ
 */
StageSelectScene::StageSelectScene(SceneManager* pSceneManager, UserResources* pUserResources)
	: Scene{ pSceneManager,pUserResources }
	, m_selectNum{ 0 }
{

}



/**
 * @brief デストラクタ
 */
StageSelectScene::~StageSelectScene()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageSelectScene::Initialize()
{
	m_selectNum = 0;
}



/**
 * @brief 更新処理
 *
 * @param[in] inputDevice 入力デバイス
 *
 * @return なし
 */
void StageSelectScene::Update(float elapsedTime)
{
	Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	if (kb->pressed.D)
	{
		m_selectNum++;
		if (m_selectNum == STAGES) m_selectNum = 0;
	}
	else if (kb->pressed.A)
	{
		m_selectNum--;
		if (m_selectNum < 0) m_selectNum = STAGES - 1;
	}

	// シーンの切り替え
	if (kb->pressed.Space || kb->pressed.P)
	{
		//ChangeScene("TestScene");

		std::string stageName = "Stage" + std::to_string(m_selectNum) + "Scene";
		ChangeScene(stageName);
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageSelectScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"StageSelectScene");
	debugFont->AddString(0, 60, Colors::Yellow, L"SelectNum:%d",m_selectNum);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageSelectScene::Finalize()
{
	
}
