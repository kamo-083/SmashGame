/**
 * @file   TitleScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/UI/UITextureCatalog.h"
#include "Source/Game/Transition/BlockTransition.h"
#include "Source/Game/GameObjects/Background.h"
#include "Source/Game/UI/UITextureCatalog.h"
#include "Source/Game/UI/Elements/UIWidget.h"
#include "Source/Game/UI/Displays/Button.h"
#include "Source/Game/UI/Displays/InputHintUI.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSM	シーンマネージャのポインタ
 * @param pUR	リソースマネージャのポインタ
 */
TitleScene::TitleScene(SceneManager* pSM, UserResources* pUR)
	:
	Scene{ pSM,pUR },
	m_textureCatalog(pSM->GetUITextureCatalog()),
	m_selectButton{ 0 }
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
		Easing::EaseType::OutBounce,
		Easing::PlaybackMode::Once
	};
	m_titleLogo->Initialize(
		m_textureCatalog->GetTextures().text_titleLogo.texture,
		data,
		m_textureCatalog->GetTextures().text_titleLogo.size);

	// 選択ボタンを作成
	SetupBotton(halfWidth);

	// 選択中のボタンを初期化
	m_selectButton = 0;

	// 背景の作成
	SetupBackground(
		m_userResources->GetDeviceResources(), m_userResources->GetShaderManager(),
		m_userResources->GetResourceManager(),
		DirectX::SimpleMath::Vector2{ static_cast<float>(windowSize.right), static_cast<float>(windowSize.bottom) }
	);

	// 操作UIの作成
	SetupInputUI();

	// BGM・SEの読み込み
	AudioManager* pAM = m_userResources->GetAudioManager();
	SetupAudio(pAM);

	// シーン遷移演出を開く
	BlockTransition* transition = m_sceneManager->GetTransition();
	if (transition->IsClose())	transition->Open();
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

	// 選択ボタンの切り替え
	if (kb->pressed.Down)
	{
		// 選択ボタンを下へ
		SelectButtonDown();

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}
	else if (kb->pressed.Up)
	{
		// 選択ボタンを上へ
		SelectButtonUp();

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}

	// 選択ボタンの決定
	if (m_userResources->GetKeyboardTracker()->pressed.Space)
	{
		m_buttons[m_selectButton]->Press();
	}

	// UIの更新
	m_titleLogo->Update(elapsedTime);
	m_buttons[m_selectButton]->Update(elapsedTime);

	// 背景の更新
	m_background->Update(elapsedTime);

	// シーン遷移演出
	BlockTransition* transition = m_sceneManager->GetTransition();
	if (transition->IsClose() && transition->IsEnd())
	{
		// 演出が終わっていたらシーン移動
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
void TitleScene::Render(RenderContext& context, DebugFont* debugFont)
{
	// デバッグ用情報追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"TitleScene");

	// 背景の描画
	m_background->Draw(context);

	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp());

	// 操作方法の描画
	for (auto& inputUI : m_inputHintUI)
	{
		inputUI->Draw(context);
	}

	// タイトルロゴの描画
	m_titleLogo->Draw(context.spriteBatch);

	// 選択肢ボタンの描画
	for (auto& button : m_buttons)
	{
		button->Draw(context);
	}

	context.spriteBatch->End();
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
	if (m_titleLogo) m_titleLogo->Finalize();
	m_titleLogo.reset();

	for (auto& button : m_buttons)
	{
		button->Finalize();
	}
	m_buttons.clear();

	for (auto& inputUI : m_inputHintUI)
	{
		inputUI->Finalize();
	}
	m_inputHintUI.clear();

	m_background.reset();
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

/**
 * @brief 選択ボタンの変更(上)
 *
 * @param なし
 *
 * @return なし
 */
void TitleScene::SelectButtonUp()
{
	// 以前選択していたボタンのアニメーションをリセット
	ButtonReset(m_selectButton);

	// 選択しているボタンを変更
	m_selectButton--;
	if (m_selectButton < 0) m_selectButton = BUTTONS - 1;
}

/**
 * @brief 選択ボタンの変更(下)
 *
 * @param なし
 *
 * @return なし
 */
void TitleScene::SelectButtonDown()
{
	// 以前選択していたボタンのアニメーションをリセット
	ButtonReset(m_selectButton);

	// 選択しているボタンを変更	
	m_selectButton++;
	if (m_selectButton == BUTTONS) m_selectButton = 0;
}

/**
 * @brief ボタンの設定
 *
 * @param windowHalfWidth ウィンドウの幅の半分
 * 
 * @return なし
 */
void TitleScene::SetupBotton(float windowHalfWidth)
{
	// オーディオマネージャーのポインタを取得
	AudioManager* pAM = m_userResources->GetAudioManager();

	m_buttons.reserve(BUTTONS);
	// ゲーム開始のボタン
	// トゥイーンアニメーションのパラメータ作成
	std::unique_ptr<Button> start = std::make_unique<Button>();
	Tween2D::TweenData data =
	{
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(windowHalfWidth, TEXT_POS_Y),
						  DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
		Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),
						  DirectX::SimpleMath::Vector2(TWEEN_DELTA_SCALE, TWEEN_DELTA_SCALE),
						  0.0f, 0.0f},
		TWEEN_ANIM_TIME,
		Easing::EaseType::OutQuart,
		Easing::PlaybackMode::PingPong
	};
	start->Initialize(
		m_textureCatalog->GetTextures().text_titleStart.texture,
		data,
		m_textureCatalog->GetTextures().text_titleStart.size,
		[this, pAM]() {
			// シーン遷移演出
			BlockTransition* transition = m_sceneManager->GetTransition();
			if (transition->IsOpen())
			{
				// SEの再生
				pAM->Play("cursorSE", false);
				// シーンを閉じる
				transition->Close();
			}
		}
	);
	m_buttons.push_back(std::move(start));

	// ゲーム終了のボタン
	std::unique_ptr<Button> exit = std::make_unique<Button>();
	// パラメータの表示位置を変更
	data.start.pos = DirectX::SimpleMath::Vector2(windowHalfWidth, TEXT_POS_Y + TEXT_INTERVAL);
	exit->Initialize(
		m_textureCatalog->GetTextures().text_titleExit.texture,
		data,
		m_textureCatalog->GetTextures().text_titleExit.size,
		[this, pAM]() {
			if (pAM->IsPlaying("title_selectBGM")) pAM->Stop("title_selectBGM");
			PostQuitMessage(0);
		}
	);
	m_buttons.push_back(std::move(exit));
}

/**
 * @brief 背景の設定
 *
 * @param pDR		 デバイスリソースのポインタ
 * @param pSM		 シェーダーマネージャーのポインタ
 * @param pRM		 リソースマネージャーのポインタ
 * @param windowSize ウィンドウサイズ
 *
 * @return なし
 */
void TitleScene::SetupBackground(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM,
	const DirectX::SimpleMath::Vector2& windowSize)
{
	m_background = std::make_unique<Background>(
		pDR, pSM, pRM,
		Background::ResourcesDesc{ "backgroundVS","Resources/Shaders/Background/BackgroundVS.cso" },
		Background::ResourcesDesc{ "backgroundPS","Resources/Shaders/Background/BackgroundPS.cso" },
		Background::ResourcesDesc{ "backgroundGS","Resources/Shaders/Background/BackgroundGS.cso" },
		Background::ResourcesDesc{ "background_dot", "Others/dot_pattern.png" },
		windowSize, Background::MoveDirection::DOWN + Background::MoveDirection::RIGHT
	);
}

/**
 * @brief 音声の設定
 *
 * @param pAM オーディオマネージャーのポインタ
 *
 * @return なし
 */
void TitleScene::SetupAudio(AudioManager* pAM)
{
	pAM->LoadMP3("title_selectBGM", "BGM/iwashiro_hitoiki_coffee.mp3");
	pAM->LoadMP3("cursorSE", "SE/button68.mp3");

	// BGM・SEの音量変更
	pAM->SetVolume("title_selectBGM", BGM_VOLUME);

	// BGMの再生
	if (!pAM->IsPlaying("title_selectBGM")) pAM->Play("title_selectBGM", true);
}

/**
 * @brief 操作UIの設定
 *
 * @param なし
 *
 * @return なし
 */
void TitleScene::SetupInputUI()
{
	std::unique_ptr<InputHintUI> inputUI;

	// 画像を設定
	InputHintUI::Textures textures;
	textures.key = m_textureCatalog->GetTextures().text_keys.texture;
	textures.action = m_textureCatalog->GetTextures().text_keyAction.texture;

	// 操作キー配列
	std::vector<DirectX::Keyboard::Keys> keys;

	// 表示位置の初期設定
	DirectX::SimpleMath::Vector2 pos = INPUT_TEXT_POS;

	// キーの文字サイズを取得
	long keyTextSize = static_cast<long>(m_textureCatalog->GetTextures().text_keys.size.y);

	// 選択
	// キーを設定
	keys.push_back(DirectX::Keyboard::Keys::Up);
	keys.push_back(DirectX::Keyboard::Keys::Down);
	// UIを作成
	inputUI = std::make_unique<InputHintUI>();
	inputUI->Initialize(
		textures, pos, INPUT_TEXT_SCALE, keyTextSize, keys,
		ActionAtlas::ActionType::SELECT);
	// 表示位置をずらす
	pos.x += inputUI->GetWidth() + INPUT_TEXT_POS_ADJUST;
	// 配列に追加
	m_inputHintUI.push_back(std::move(inputUI));

	// 決定
	// キーを設定
	keys.clear();
	keys.push_back(DirectX::Keyboard::Keys::Space);
	// UIを作成
	inputUI = std::make_unique<InputHintUI>();
	inputUI->Initialize(
		textures, pos, INPUT_TEXT_SCALE, keyTextSize, keys,
		ActionAtlas::ActionType::DECIDE);
	// 配列に追加
	m_inputHintUI.push_back(std::move(inputUI));
}