/**
 * @file   TitleScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"
#include "Source/Game/UI/UIWidget.h"
#include "Source/Game/UI/Button.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSM    シーンを管理しているマネージャ
 * @param pUR	 リソースを管理しているマネージャ
 */
TitleScene::TitleScene(SceneManager* pSM, UserResources* pUR)
	: Scene{ pSM,pUR }
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
 * @param なし
 *
 * @return なし
 */
void TitleScene::Initialize()
{
	// テクスチャの読み込み
	ResourceManager* pRM = m_userResources->GetResourceManager();
	m_textures = std::make_unique<Textures>();
	m_textures->logo = pRM->RequestPNG("titleLogo", "Text/titleLogo.png");
	m_textures->start = pRM->RequestPNG("startText", "Resources/Textures/Text/startText.png");
	m_textures->exit = pRM->RequestPNG("exitText", "Resources/Textures/Text/exitText.png");
	m_textures->background = pRM->RequestPNG("background2D", "Resources/Textures/Others/background.png");

	// ウィンドウサイズの取得
	RECT windowSize = m_userResources->GetDeviceResources()->GetOutputSize();
	float halfWidth = windowSize.right / 2.0f;

	// タイトルロゴを作成
	m_titleLogo = std::make_unique<UIWidget>();
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(halfWidth, 0.0f),
						  DirectX::SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, LOGO_POS_Y),
						  DirectX::SimpleMath::Vector2(0.0f,0.0f),0.0f,0.0f},
		1.0f,
		Tween2D::Ease::OutBounce,
		Tween2D::PlaybackMode::Once
	};
	m_titleLogo->Initialize(m_textures->logo, data, LOGO_SIZE);

	// ボタンを作成
	m_buttons.reserve(BUTTONS);
	// ゲーム開始のボタン
	// トゥイーンアニメーションのパラメータ作成
	std::unique_ptr<Button> start = std::make_unique<Button>();
	data =
	{
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(halfWidth, TEXT_POS_Y),
						  DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),
						  DirectX::SimpleMath::Vector2(TWEEN_DELTA_SCALE, TWEEN_DELTA_SCALE),
						  0.0f, 0.0f},
		TWEEN_ANIM_TIME,
		Tween2D::Ease::OutQuart,
		Tween2D::PlaybackMode::PingPong
	};
	start->Initialize(
		m_textures->start, data, TEXT_SIZE,
		[this]() {
			// SEの再生
			m_userResources->GetAudioManager()->Play("cursorSE", false);

			// シーン切り替え
			ChangeScene("StageSelectScene");
		}
	);
	m_buttons.push_back(std::move(start));

	// ゲーム終了のボタン
	std::unique_ptr<Button> exit = std::make_unique<Button>();
	// パラメータの表示位置を変更
	data.start.pos = DirectX::SimpleMath::Vector2(halfWidth, TEXT_POS_Y + TEXT_INTERVAL);
	exit->Initialize(
		m_textures->exit, data, TEXT_SIZE,
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
	pAM->LoadMP3("title_selectBGM", "BGM/iwashiro_hitoiki_coffee.mp3");
	pAM->LoadMP3("cursorSE", "SE/button68.mp3");

	// BGM・SEの音量変更
	pAM->SetVolume("title_selectBGM", BGM_VOLUME);

	// BGMの再生
	if (!pAM->IsPlaying("title_selectBGM")) pAM->Play("title_selectBGM", true);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void TitleScene::Update(float elapsedTime)
{
	DirectX::Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	// ボタン切り替え
	if (kb->pressed.Down)
	{
		ButtonReset(m_selectButton);
		m_selectButton++;
		if (m_selectButton == BUTTONS) m_selectButton = 0;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}
	else if (kb->pressed.Up)
	{
		ButtonReset(m_selectButton);
		m_selectButton--;
		if (m_selectButton < 0) m_selectButton = BUTTONS - 1;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
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
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void TitleScene::Render(RenderContext context, DebugFont* debugFont)
{
	// デバッグ用情報追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"TitleScene");

	// 背景の描画
	context.spriteBatch->Begin();
	context.spriteBatch->Draw(m_textures->background, DirectX::SimpleMath::Vector2::Zero);
	context.spriteBatch->End();

	// タイトルロゴの描画
	m_titleLogo->Draw(context);

	// 選択肢ボタンの描画
	for (auto& button : m_buttons)
	{
		button->Draw(context);
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
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



/**
 * @brief 指定したボタンのアニメーションをリセット
 *
 * @param buttonNum	ボタン番号
 *
 * @return なし
 */
void TitleScene::ButtonReset(int buttonNum)
{
	m_buttons[buttonNum]->Reset();
	m_buttons[buttonNum]->Update(0.0f);
}
