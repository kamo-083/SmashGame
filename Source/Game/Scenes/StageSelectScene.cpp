/**
 * @file   StageSelectScene.cpp
 *
 * @brief  ステージ選択シーンに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageSelectScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/UI/UITextureCatalog.h"
#include "Source/Game/Transition/BlockTransition.h"
#include "Source/Game/UI/Displays/Button.h"
#include "Source/Game/UI/Elements/NumberRenderer/NumberRenderer2D.h"
#include "Source/Game/UI/Displays/InputHintUI.h"
#include "Source/Game/Common/RenderTexture.h"
#include "Source/Game/GameObjects/Background.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSM		シーンマネージャのポインタ
 * @param pUR		リソースマネージャのポインタ
 * @param stages	ステージ数
 */
StageSelectScene::StageSelectScene(SceneManager* pSM, UserResources* pUR, int stages)
	: 
	Scene{ pSM,pUR },
	STAGES{ stages },
	m_textureCatalog(pSM->GetUITextureCatalog()),
	m_selectStage{ 0 },
	m_transitionStage{ -1 }
{
	m_stageCleared.resize(STAGES);
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
 * @param なし
 *
 * @return なし
 */
void StageSelectScene::Initialize()
{
	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().right),
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().bottom)
	);

	// ステージのクリア状況を設定
	for (int i = 0; i < STAGES; i++)
	{
		std::string data = m_sceneManager->GetSharedData("cleared" + std::to_string(i + 1));
		if (data == "true")	m_stageCleared[i] = true;	// クリア済み
		else				m_stageCleared[i] = false;	// 未クリア
	}

	// ステージ番号表示オブジェクトの作成
	SetupNumberBoard();

	// ステージ選択パネルの作成
	SetupPanel(windowSize);

	// 背景の作成
	SetupBackground(
		m_userResources->GetDeviceResources(), m_userResources->GetShaderManager(),
		m_userResources->GetResourceManager(), windowSize
	);

	// 音声の設定
	AudioManager* pAM = m_userResources->GetAudioManager();
	SetupAudios(pAM);

	// 操作UIの作成
	SetupInputUI();

	// シーン遷移演出を開く
	BlockTransition* transition = m_sceneManager->GetTransition();
	if (transition->IsClose())	transition->Open();

	// 遷移先のステージ番号を初期化
	m_transitionStage = -1;
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void StageSelectScene::Update(float elapsedTime)
{
	DirectX::Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	// Pキーでクリア状況をリセット
	if (kb->pressed.P) ResetStages();

	// 選択ステージ切り替え
	ChangeSelectStage(kb);

	// ステージパネルの更新
	m_stagePanels[m_selectStage]->Update(elapsedTime);

	// 背景の更新
	m_background->Update(elapsedTime);

	// シーン移動
	TransitionScene(kb);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont デバッグ用フォント
 *
 * @return なし
 */
void StageSelectScene::Render(RenderContext& context, DebugFont* debugFont)
{
	// デバッグ用情報の追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageSelectScene");
	debugFont->AddString(0, 60, DirectX::Colors::Yellow, L"Select:%d", m_selectStage);

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

	// パネルの描画
	for (auto& panel : m_stagePanels)
	{
		panel->Draw(context);
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
void StageSelectScene::Finalize()
{
	for (auto& panel : m_stagePanels)
	{
		panel->Finalize();
	}
	m_stagePanels.clear();
	
	for (auto& inputUI : m_inputHintUI)
	{
		inputUI->Finalize();
	}
	m_inputHintUI.clear();

	for (auto& renderTexture : m_renderTextures)
	{
		renderTexture->Finalize();
	}
	m_renderTextures.clear();

	m_background.reset();
}



/**
 * @brief 指定したステージパネルのアニメーションをリセット
 *
 * @param panelNum	パネル番号
 *
 * @return なし
 */
void StageSelectScene::ResetPanel(int panelNum)
{
	m_stagePanels[panelNum]->Reset();
	m_stagePanels[panelNum]->Update(0.0f);
}



/**
 * @brief 選択中のステージを切り替え
 *
 * @param kb キーボードトラッカーのポインタ
 *
 * @return なし
 */
void StageSelectScene::ChangeSelectStage(DirectX::Keyboard::KeyboardStateTracker* kb)
{
	// オーディオマネージャーのポインタを取得
	AudioManager* pAM = m_userResources->GetAudioManager();

	if (kb->pressed.Right)	// 右へ
	{
		// 前に選択していたステージのパネルをリセット
		ResetPanel(m_selectStage);

		m_selectStage++;
		if (m_selectStage == STAGES) m_selectStage = 0;

		// SEの再生
		pAM->Play("cursorSE", false);
	}
	else if (kb->pressed.Left)	// 左へ
	{
		// 前に選択していたステージのパネルをリセット
		ResetPanel(m_selectStage);

		m_selectStage--;
		if (m_selectStage < 0) m_selectStage = STAGES - 1;

		// SEの再生
		pAM->Play("cursorSE", false);
	}
}



/**
 * @brief シーンを移動
 *
 * @param kb キーボードトラッカーのポインタ
 *
 * @return なし
 */
void StageSelectScene::TransitionScene(DirectX::Keyboard::KeyboardStateTracker* kb)
{
	// シーン遷移演出クラスのポインタを取得
	BlockTransition* transition = m_sceneManager->GetTransition();
	// オーディオマネージャーのポインタを取得
	AudioManager* pAM = m_userResources->GetAudioManager();

	if (kb->pressed.Space)	// 各ステージへ
	{
		m_stagePanels[m_selectStage]->Press();
	}
	if (kb->pressed.Q)		// タイトル画面へ
	{
		transition->Close();
	}

	// シーン遷移演出
	if (kb->pressed.Q && transition->IsOpen())
	{
		// シーンを閉じる
		transition->Close();
	}
	if (transition->IsClose() && transition->IsEnd())	// シーン遷移演出が終わっていたら
	{
		if (m_transitionStage != -1)	// 遷移先のステージ番号が登録されていた場合
		{
			// 番号からシーン名を作成
			std::string stageName = "Stage" + std::to_string(m_transitionStage + 1) + "Scene";
			// BGMの停止
			if (pAM->IsPlaying("title_selectBGM")) pAM->Stop("title_selectBGM");

			ChangeScene(stageName);		// 各ステージへ
		}
		else
		{
			ChangeScene("TitleScene");	// タイトルへ
		}
	}
}



/**
 * @brief パネルの画像を合成
 *
 * @param なし
 *
 * @return なし
 */
void StageSelectScene::PanelTextureSynthesis()
{
	// 構造体を設定
	RenderContext context;
	context.deviceContext = m_userResources->GetDeviceResources()->GetD3DDeviceContext();
	context.states = m_userResources->GetStates();
	context.spriteBatch = m_userResources->GetSpriteBatch();

	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().right),
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().bottom)
	);

	// ベース描画の設定
	DirectX::SimpleMath::Vector2 base_size = m_textureCatalog->GetTextures().window_stageSelect.size;
	DirectX::SimpleMath::Vector2 base_pos = base_size * 0.5f;
	RECT base_rect = {};
	base_rect.right = static_cast<LONG>(base_size.x);
	base_rect.bottom = static_cast<LONG>(base_size.y);

	// スタンプ画像の描画範囲 (画像全体をそのまま)
	DirectX::SimpleMath::Vector2 stamp_size = m_textureCatalog->GetTextures().icon_stampOn.size;
	RECT stamp_rect = {};
	stamp_rect.right = static_cast<LONG>(stamp_size.x);
	stamp_rect.bottom = static_cast<LONG>(stamp_size.y);

	// レンダーターゲット初期化用
	const float clear[4] = { 0,0,0,0 };

	// 各要素の合成
	for (int i = 0; i < STAGES; i++)
	{
		// レンダーターゲットを初期化
		m_renderTextures[i]->Clear(context.deviceContext, clear);

		// オフスクリーンに設定
		m_renderTextures[i]->SetRTVTexture(context.deviceContext, nullptr);

		// パネルのスケールを取得
		float scale = m_stagePanels[i]->GetParam().scale.x;

		// ステージ番号
		DirectX::SimpleMath::Vector2 number_pos = DirectX::SimpleMath::Vector2(
			base_size.x * 0.75f,
			NUMBER_ADJUST_HEIGHT
		);
		// ステージ番号描画の設定
		m_numberBoard->SetNumber(i + 1);
		m_numberBoard->SetPosition(number_pos);
		m_numberBoard->SetScale(scale);

		// スタンプ描画の設定
		DirectX::SimpleMath::Vector2 stamp_pos = base_pos;
		stamp_pos.y += stamp_size.y * 0.25f;
		ID3D11ShaderResourceView* stamp_tex = nullptr;	// 画像のポインタ
		DirectX::SimpleMath::Color stamp_color;			// 画像の色
		if (m_stageCleared[i])	// クリア済みかどうか
		{
			// スタンプ
			stamp_tex = m_textureCatalog->GetTextures().icon_stampOn.texture.Get();
			stamp_color = DirectX::Colors::Red;
		}
		else
		{
			// 枠のみ
			stamp_tex = m_textureCatalog->GetTextures().icon_stampOff.texture.Get();
			stamp_color = DirectX::Colors::Gray;
		}

		context.spriteBatch->Begin(
			DirectX::SpriteSortMode_Deferred,
			context.states->NonPremultiplied(),
			context.states->LinearClamp());
		// ベースを描画
		context.spriteBatch->Draw(
		m_textureCatalog->GetTextures().window_stageSelect.texture.Get(),
		base_pos, &base_rect, DirectX::Colors::White, 0.0f, base_size * 0.5f, scale);
		// ステージ番号を描画
		m_numberBoard->Draw(context);
		// スタンプを描画
		context.spriteBatch->Draw(stamp_tex, stamp_pos, &stamp_rect, stamp_color, 0.0f, stamp_size * 0.5f, scale);
		context.spriteBatch->End();

		// 通常の画面に設定
		m_renderTextures[i]->SetRTVDefault(context.deviceContext, nullptr);

		// パネルに画像を設定
		m_stagePanels[i]->SetTexture(m_renderTextures[i]->GetSRV());
	}

}



/**
 * @brief ステージパネルの設定
 *
 * @param windowSize	ウィンドウサイズ
 *
 * @return なし
 */
void StageSelectScene::SetupPanel(const DirectX::SimpleMath::Vector2& windowSize)
{
	DX::DeviceResources* pDR = m_userResources->GetDeviceResources();
	DirectX::SimpleMath::Vector2 panelSize = m_textureCatalog->GetTextures().window_stageSelect.size;

	// ステージ数分設定
	for (int i = 0; i < STAGES; i++)
	{
		// 表示位置を計算
		DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(
			windowSize.x / static_cast<float>(STAGES) * i + PANEL_ADJUST_INTERVAL,
			windowSize.y * 0.5f
		);

		// アニメーションのパラメータを設定
		Tween2D::TweenData data =
		{
			Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
			Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),
							  DirectX::SimpleMath::Vector2(PANEL_DELTA_SCALE, PANEL_DELTA_SCALE),
							  0.0f, 0.0f},
			PANEL_ANIM_TIME,
			Easing::EaseType::OutQuart,
			Easing::PlaybackMode::PingPong
		};

		// パネルを作成・追加
		std::unique_ptr<Button> panel = std::make_unique<Button>();
		panel->Initialize(
			m_textureCatalog->GetTextures().window_stageSelect.texture,
			data, m_textureCatalog->GetTextures().window_stageSelect.size,
			[this, i]() {
				BlockTransition* transition = m_sceneManager->GetTransition();
				if (transition->IsOpen())
				{
					// シーンを閉じる
					transition->Close();

					// 遷移先のステージ番号を保存
					m_transitionStage = i;
				}
			});
		m_stagePanels.push_back(std::move(panel));

		// 合成用レンダーテクスチャの作成
		std::unique_ptr<RenderTexture> renderTexture = std::make_unique<RenderTexture>();
		renderTexture->Initialize(
			pDR->GetD3DDevice(),
			static_cast<int>(panelSize.x), static_cast<int>(panelSize.y),
			pDR->GetRenderTargetView(),
			pDR->GetDepthStencilView()
		);
		m_renderTextures.push_back(std::move(renderTexture));
	}

	PanelTextureSynthesis();
}



/**
 * @brief ステージ番号表示の設定
 *
 * @param なし
 *
 * @return なし
 */
void StageSelectScene::SetupNumberBoard()
{
	m_numberBoard = std::make_unique<NumberRenderer2D>(
		INumberRenderer::DisplayMode::Default,
		m_textureCatalog->GetTextures().text_number.size,
		m_textureCatalog->GetTextures().text_number.texture,
		1);
	m_numberBoard->SetUseBeginEnd(false);
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
void StageSelectScene::SetupBackground(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM,
	DirectX::SimpleMath::Vector2 windowSize)
{
	m_background = std::make_unique<Background>(
		pDR, pSM, pRM,
		Background::ResourcesDesc{ "backgroundVS","Resources/Shaders/Background/BackgroundVS.cso" },
		Background::ResourcesDesc{ "backgroundPS","Resources/Shaders/Background/BackgroundPS.cso" },
		Background::ResourcesDesc{ "backgroundGS","Resources/Shaders/Background/BackgroundGS.cso" },
		Background::ResourcesDesc{ "background_stripes", "Others/stripes_pattern.png" },
		windowSize, Background::MoveDirection::RIGHT
	);
}



/**
 * @brief 音声の設定
 *
 * @param pAM オーディオマネージャーのポインタ
 *
 * @return なし
 */
void StageSelectScene::SetupAudios(AudioManager* pAM)
{
	// BGM・SEの読み込み
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
void StageSelectScene::SetupInputUI()
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
	keys.push_back(DirectX::Keyboard::Keys::Left);
	keys.push_back(DirectX::Keyboard::Keys::Right);
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
	// 表示位置をずらす
	pos.x += inputUI->GetWidth() + INPUT_TEXT_POS_ADJUST;
	// 配列に追加
	m_inputHintUI.push_back(std::move(inputUI));

	// タイトルへ
	// キーを設定
	keys.clear();
	keys.push_back(DirectX::Keyboard::Keys::Q);
	// UIを作成
	inputUI = std::make_unique<InputHintUI>();
	inputUI->Initialize(
		textures, pos, INPUT_TEXT_SCALE, keyTextSize, keys,
		ActionAtlas::ActionType::TO_TITLE);
	// 配列に追加
	m_inputHintUI.push_back(std::move(inputUI));
}



/**
 * @brief ステージ状態のリセット
 *
 * @param なし
 *
 * @return なし
 */
void StageSelectScene::ResetStages()
{
	for (int i = 0; i < STAGES; i++)
	{
		std::string data = m_sceneManager->GetSharedData("cleared" + std::to_string(i + 1));
		if (data == "true")	m_sceneManager->SetSharedData("cleared" + std::to_string(i + 1), "empty");
			
		m_stageCleared[i] = false;
	}
}