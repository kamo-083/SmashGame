/**
 * @file   TitleScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"


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
	ResourceManager* pRM = m_userResources->GetResourceManager();
	m_textures = std::make_unique<Textures>();
	m_textures->logo = pRM->RequestPNG("titleLogo", L"Resources/Textures/Text/title.png");
	m_textures->start = pRM->RequestPNG("startText", L"Resources/Textures/Text/startText.png");
	m_textures->exit = pRM->RequestPNG("exitText", L"Resources/Textures/Text/exitText.png");
	m_textures->background = pRM->RequestPNG("background2D", L"Resources/Textures/background.png");

	// タイトルロゴを作成
	m_titleLogo = std::make_unique<UIWidget>();
	Tween::TweenData data =
	{
		Tween::UIParams{DirectX::SimpleMath::Vector2(640.0f, 0.0f),DirectX::SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{DirectX::SimpleMath::Vector2(0.0f, 200.0f),DirectX::SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		1.0f,
		Tween::Ease::OutBounce,
		Tween::PlaybackMode::Once
	};
	m_titleLogo->Initialize(m_textures->logo, data, DirectX::SimpleMath::Vector2(600.0f, 150.0f));

	// ボタンを作成
	m_buttons.reserve(BUTTONS);
	// ゲーム開始のボタン
	std::unique_ptr<Button> start = std::make_unique<Button>();
	data =
	{
		Tween::UIParams{DirectX::SimpleMath::Vector2(640.0f, 450.0f),DirectX::SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),DirectX::SimpleMath::Vector2(0.1f,0.1f),0.0f,0.0f},
		0.5f,
		Tween::Ease::OutQuart,
		Tween::PlaybackMode::PingPong
	};
	start->Initialize(
		m_textures->start, data, DirectX::SimpleMath::Vector2(400.0f, 67.0f),
		[this]() {ChangeScene("StageSelectScene"); }	// シーン切り替え
	);
	m_buttons.push_back(std::move(start));

	// ゲーム終了のボタン
	std::unique_ptr<Button> exit = std::make_unique<Button>();
	data =
	{
		Tween::UIParams{DirectX::SimpleMath::Vector2(640.0f, 550.0f),DirectX::SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),DirectX::SimpleMath::Vector2(0.1f,0.1f),0.0f,0.0f},
		0.5f,
		Tween::Ease::OutQuart,
		Tween::PlaybackMode::PingPong
	};
	exit->Initialize(
		m_textures->exit, data, DirectX::SimpleMath::Vector2(400.0f, 67.0f),
		[this]() {
			if (m_userResources->GetAudioManager()->IsPlaying("title_selectBGM")) m_userResources->GetAudioManager()->Stop("title_selectBGM");
			PostQuitMessage(0);
		}
	);
	m_buttons.push_back(std::move(exit));

	// 選択中のボタンを初期化
	m_selectButton = 0;

	// BGM・SEの読み込み
	AudioManager* pAM = m_userResources->GetAudioManager();
	pAM->LoadMP3("title_selectBGM", "Resources/Sounds/BGM/iwashiro_hitoiki_coffee.mp3");

	// BGMの再生
	if (!pAM->IsPlaying("title_selectBGM")) pAM->Play("title_selectBGM", true);
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
	DirectX::Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

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
	debugFont->AddString(0, 30, DirectX::Colors::White, L"TitleScene");

	// 背景の描画
	context.spriteBatch->Begin();
	context.spriteBatch->Draw(m_textures->background, DirectX::SimpleMath::Vector2::Zero);
	context.spriteBatch->End();

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
