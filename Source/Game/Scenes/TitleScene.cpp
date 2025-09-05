/**
 * @file   TitleScene.cpp
 *
 * @brief  ＸＸＸＸシーンに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] sceneManager    シーンを管理しているマネージャ
 * @param[in] resourceManager リソースを管理しているマネージャ
 */
TitleScene::TitleScene(SceneManager* pSceneManager, UserResources* pUserResources)
	: Scene{ pSceneManager,pUserResources }
	, m_logoTexture{ nullptr }
{

}



/**
 * @brief デストラクタ
 */
TitleScene::~TitleScene()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Initialize()
{
	m_logoTexture = m_userResources->GetResourceManager()->RequestTexture("titleLogo", L"Resources/Textures/Text/title.png");
}



/**
 * @brief 更新処理
 *
 * @param[in] inputDevice 入力デバイス
 *
 * @return なし
 */
void TitleScene::Update(float elapsedTime)
{
	// キーボードの更新
	auto kb = Keyboard::Get().GetState();
	m_kbTracker.Update(kb);

	// シーンの切り替え
	if (m_kbTracker.pressed.P)
	{
		ChangeScene("StageSelectScene");
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"TitleScene");

	context.spriteBatch->Begin();

	context.spriteBatch->Draw(m_logoTexture, DirectX::SimpleMath::Vector2(640.0f, 100.0f));

	context.spriteBatch->End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Finalize()
{
	m_logoTexture = nullptr;
}
