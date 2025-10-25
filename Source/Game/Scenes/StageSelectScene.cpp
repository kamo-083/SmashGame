/**
 * @file   StageSelectScene.cpp
 *
 * @brief  ステージ選択シーンに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageSelectScene.h"
#include "Source/Game/UI/Button.h"
#include "Source/Game/UI/NumberRenderer/NumberRenderer2D.h"
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
	for (int i = 0; i < STAGES; i++)
	{
		DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2(
			windowSize.x / static_cast<float>(STAGES) * i + PANEL_ADJUST_INTERVAL,
			windowSize.y * 0.5f
		);

		Tween2D::TweenData data =
		{
			Tween2D::UIParams{pos,DirectX::SimpleMath::Vector2(1.0f,1.0f), 0.0f, 1.0f},
			Tween2D::UIParams{DirectX::SimpleMath::Vector2(0.0f, 0.0f),
							  DirectX::SimpleMath::Vector2(PANEL_DELTA_SCALE, PANEL_DELTA_SCALE),
							  0.0f, 0.0f},
			PANEL_ANIM_TIME,
			Tween2D::Ease::OutQuart,
			Tween2D::PlaybackMode::PingPong
		};

		std::unique_ptr<Button> panel = std::make_unique<Button>();
		panel->Initialize(
			pRM->RequestPNG("stagePanel", "UI/stagePanel.png"),
			data, PANEL_TEX_SIZE,
			[this, i]() {
				// BGMの停止
				if (m_userResources->GetAudioManager()->IsPlaying("title_selectBGM")) m_userResources->GetAudioManager()->Stop("title_selectBGM");

				// 選択番号からシーン名を作成
				std::string stageName = "Stage" + std::to_string(i + 1) + "Scene";
				ChangeScene(stageName);
			});
		m_stagePanels.push_back(std::move(panel));
	}

	// ステージ番号表示オブジェクトの作成
	m_numberBoard = std::make_unique<NumberRenderer2D>(
		NUMBER_SIZE,
		pRM->RequestPNG("number", "Text/number_48.png"),
		1);
	m_numberBoard->SetUseBeginEnd(false);

	// テクスチャの読み込み
	m_textures = std::make_unique<Textures>();
	m_textures->background = pRM->RequestPNG("background2D", "Others/background.png");

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
void StageSelectScene::Update(float elapsedTime)
{
	DirectX::Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	// ステージ切り替え
	if (kb->pressed.Right)
	{
		// 前に選択していたステージのパネルをリセット
		PanelReset(m_selectNum);

		m_selectNum++;
		if (m_selectNum == STAGES) m_selectNum = 0;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}
	else if (kb->pressed.Left)
	{
		// 前に選択していたステージのパネルをリセット
		PanelReset(m_selectNum);

		m_selectNum--;
		if (m_selectNum < 0) m_selectNum = STAGES - 1;

		// SEの再生
		m_userResources->GetAudioManager()->Play("cursorSE", false);
	}

	// ステージパネルの更新
	m_stagePanels[m_selectNum]->Update(elapsedTime);

	// シーン移動
	if (kb->pressed.Space)	// 各ステージへ
	{
		m_stagePanels[m_selectNum]->Press();
	}
	if (kb->pressed.Q)		// タイトル画面へ
	{
		ChangeScene("TitleScene");
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
void StageSelectScene::Render(RenderContext context, DebugFont* debugFont)
{
	// デバッグ用情報の追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageSelectScene");
	debugFont->AddString(0, 60, DirectX::Colors::Yellow, L"Select:%d",m_selectNum);

	// 背景の描画
	context.spriteBatch->Begin();
	context.spriteBatch->Draw(m_textures->background, DirectX::SimpleMath::Vector2::Zero);
	context.spriteBatch->End();

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

	context.spriteBatch->Begin();

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
void StageSelectScene::PanelReset(int panelNum)
{
	m_stagePanels[panelNum]->Reset();
	m_stagePanels[panelNum]->Update(0.0f);
}