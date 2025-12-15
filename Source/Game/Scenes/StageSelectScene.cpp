/**
 * @file   StageSelectScene.cpp
 *
 * @brief  ステージ選択シーンに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageSelectScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Transition/BlockTransition.h"
#include "Source/Game/UI/Displays/Button.h"
#include "Source/Game/UI/Elements/NumberRenderer/NumberRenderer2D.h"
#include "Source/Game/Common/RenderTexture.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSM		シーンを管理しているマネージャ
 * @param pUR		リソースを管理しているマネージャ
 * @param stages	ステージ数
 */
StageSelectScene::StageSelectScene(SceneManager* pSM, UserResources* pUR, int stages)
	: Scene{ pSM,pUR }
	, STAGES{ stages }
	, m_selectStage{ 0 }
	, m_transitionStage{ -1 }
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

	ResourceManager* pRM = m_userResources->GetResourceManager();

	// ステージ選択パネルの作成
	SetupPanel(windowSize, pRM);

	// ステージ番号表示オブジェクトの作成
	SetupNumberBoard(pRM);

	// テクスチャの読み込み
	SetupTextures(pRM);

	// 音声の設定
	AudioManager* pAM = m_userResources->GetAudioManager();
	SetupAudios(pAM);

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

	// 選択ステージ切り替え
	ChangeSelectStage(kb);

	// ステージパネルの更新
	m_stagePanels[m_selectStage]->Update(elapsedTime);

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
void StageSelectScene::Render(RenderContext context, DebugFont* debugFont)
{
	// デバッグ用情報の追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageSelectScene");
	debugFont->AddString(0, 60, DirectX::Colors::Yellow, L"Select:%d",m_selectStage);

	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp());

	// 背景の描画
	context.spriteBatch->Draw(m_textures->background, DirectX::SimpleMath::Vector2::Zero);

	// 操作方法の描画
	context.spriteBatch->Draw(m_textures->key, DirectX::SimpleMath::Vector2::Zero, &KEY_RECT);
	// 2行目
	DirectX::SimpleMath::Vector2 keyPos = { static_cast<float>(KEY_RECT.right),0.0f };
	RECT keyRect = { 0,KEY_RECT.bottom,KEY_RECT.right,KEY_RECT.bottom + KEY_HEIGHT };
	context.spriteBatch->Draw(m_textures->key, keyPos, &keyRect);

	// パネルの描画
	for (auto& panel : m_stagePanels)
	{
		panel->Draw(context);
	}

	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().right),
		static_cast<float>(m_userResources->GetDeviceResources()->GetOutputSize().bottom)
	);

	// ステージ番号の描画
	for (int i = 0; i < STAGES; i++)
	{
		DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(
			windowSize.x / static_cast<float>(STAGES) * i + NUMBER_ADJUST_INTERVAL,
			windowSize.y * 0.25f
		);
		m_numberBoard->SetNumber(i + 1);
		m_numberBoard->SetPosition(pos);
		m_numberBoard->SetScale(m_stagePanels[i]->GetParam().scale.x);
		m_numberBoard->Draw(context);
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
	if (kb->pressed.Right)	// 右へ
	{
		// 前に選択していたステージのパネルをリセット
		ResetPanel(m_selectStage);

		m_selectStage++;
		if (m_selectStage == STAGES) m_selectStage = 0;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}
	else if (kb->pressed.Left)	// 左へ
	{
		// 前に選択していたステージのパネルをリセット
		ResetPanel(m_selectStage);

		m_selectStage--;
		if (m_selectStage < 0) m_selectStage = STAGES - 1;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
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
			if (m_userResources->GetAudioManager()->IsPlaying("title_selectBGM")) m_userResources->GetAudioManager()->Stop("title_selectBGM");

			ChangeScene(stageName);		// 各ステージへ
		}
		else
		{
			ChangeScene("TitleScene");	// タイトルへ
		}
	}
}



/**
 * @brief ステージパネルの設定
 *
 * @param windowSize	ウィンドウサイズ
 * @param pRM			リソースマネージャーのポインタ	
 *
 * @return なし
 */
void StageSelectScene::SetupPanel(DirectX::SimpleMath::Vector2 windowSize, ResourceManager* pRM)
{
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
			pRM->RequestPNG("stagePanel", "UI/stagePanel.png"),
			data, PANEL_TEX_SIZE,
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
	}
}



/**
 * @brief ステージ番号表示の設定
 *
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void StageSelectScene::SetupNumberBoard(ResourceManager* pRM)
{
	m_numberBoard = std::make_unique<NumberRenderer2D>(
		NUMBER_SIZE,
		pRM->RequestPNG("number", "Text/number_48.png"),
		1);
	m_numberBoard->SetUseBeginEnd(false);
}



/**
 * @brief テクスチャの設定
 *
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void StageSelectScene::SetupTextures(ResourceManager* pRM)
{
	m_textures = std::make_unique<Textures>();
	m_textures->background = pRM->RequestPNG("background2D", "Others/background.png");
	m_textures->key = pRM->RequestPNG("title_selectText", "Text/title_selectKeyText.png");
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
