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
	// テクスチャの読み込み
	m_textures = std::make_unique<Textures>();
	m_textures->logo = m_userResources->GetResourceManager()->RequestTexture("titleLogo", L"Resources/Textures/Text/title.png");
	m_textures->pressSpaceKey= m_userResources->GetResourceManager()->RequestTexture("pressSpaceKey", L"Resources/Textures/Text/pressSpaceKey.png");

	m_ui = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{SimpleMath::Vector2(200.0f, 300.0f),SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(200.0f, 0.0f),SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		1.0f,
		Tween::Ease::OutQuart
	};
	m_ui->Initialize(m_userResources->GetResourceManager()->RequestTexture("uiTest",L"Resources/Textures/star.png"),
					 data, SimpleMath::Vector2(100, 100));
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void TitleScene::Update(float elapsedTime)
{
	m_ui->Update(elapsedTime);

	// シーンの切り替え
	if (m_userResources->GetKeyboardTracker()->pressed.Space || m_userResources->GetKeyboardTracker()->pressed.P)
	{
		ChangeScene("StageSelectScene");
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] context	描画用構造体
 * @param[in] debugFont デバッグ用フォント
 *
 * @return なし
 */
void TitleScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"TitleScene");

	context.spriteBatch->Begin();

	context.spriteBatch->Draw(m_textures->logo, SimpleMath::Vector2(640.0f, 100.0f));
	//context.spriteBatch->Draw(m_textures->pressSpaceKey, SimpleMath::Vector2(640.0f, 300.0f));

	context.spriteBatch->End();

	m_ui->Draw(context);
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
	m_textures.reset();

	if (m_ui)m_ui->Finalize();
	m_ui.reset();
}
