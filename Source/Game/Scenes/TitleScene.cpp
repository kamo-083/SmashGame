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
	, m_selectButton{ 0 }
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

	// タイトルロゴを作成
	m_titleLogo = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{SimpleMath::Vector2(640.0f, 0.0f),SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(0.0f, 200.0f),SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		1.0f,
		Tween::Ease::OutBounce,
		false
	};
	m_titleLogo->Initialize(m_textures->logo, data, SimpleMath::Vector2(600.0f, 150.0f));

	// ボタンを作成
	m_buttons.reserve(BUTTONS);
	std::unique_ptr<Button> start = std::make_unique<Button>(); 
	data =
	{
		Tween::UIParams{SimpleMath::Vector2(640.0f, 450.0f),SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(0.0f, 0.0f),SimpleMath::Vector2(0.0f,0.0f),0.0f,-1.0f},
		2.0f,
		Tween::Ease::OutQuart,
		true
	};
	start->Initialize(
		m_textures->pressSpaceKey, data, SimpleMath::Vector2(400.0f, 105.0f),
		[this]() {ChangeScene("StageSelectScene"); }	// シーン切り替え
	);
	m_buttons.push_back(std::move(start));

	std::unique_ptr<Button> exit = std::make_unique<Button>(); 
	data =
	{
		Tween::UIParams{SimpleMath::Vector2(640.0f, 550.0f),SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{SimpleMath::Vector2(0.0f, 0.0f),SimpleMath::Vector2(0.0f,0.0f),0.0f,-1.0f},
		2.0f,
		Tween::Ease::OutQuart,
		true
	};
	exit->Initialize(
		m_textures->pressSpaceKey, data, SimpleMath::Vector2(400.0f, 105.0f),
		[this]() {PostQuitMessage(0); }		// ゲームを終了
	);
	m_buttons.push_back(std::move(exit));
	
	m_selectButton = 0;
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
	Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	// ボタン切り替え
	if (kb->pressed.S)
	{
		ButtonReset(m_selectButton);
		m_selectButton++;
		if (m_selectButton == BUTTONS) m_selectButton = 0;
	}
	else if (kb->pressed.W)
	{
		ButtonReset(m_selectButton);
		m_selectButton--;
		if (m_selectButton < 0) m_selectButton = BUTTONS - 1;
	}
	
	// ボタンの操作
	if (m_userResources->GetKeyboardTracker()->pressed.Space)
	{
		m_buttons[m_selectButton]->Press();
	}

	// UIの更新
	m_titleLogo->Update(elapsedTime);
	m_buttons[m_selectButton]->Update(elapsedTime);

	if (m_userResources->GetKeyboardTracker()->pressed.P)
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

	m_titleLogo->Draw(context);

	for (auto& button : m_buttons)
	{
		button->Draw(context);
	}
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
	if (m_titleLogo)m_titleLogo->Finalize();
	m_titleLogo.reset();

	for (auto& button : m_buttons)
	{
		button->Finalize();
	}
	m_buttons.clear();

	m_textures.reset();
}

void TitleScene::ButtonReset(int buttonNum)
{
	m_buttons[buttonNum]->Reset();
	m_buttons[buttonNum]->Update(0.0f);
}
