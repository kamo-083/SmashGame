/**
 * @file   StageSelectScene.cpp
 *
 * @brief  ステージ選択シーンに関するソースファイル
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
StageSelectScene::StageSelectScene(SceneManager* pSceneManager, UserResources* pUserResources, int stages)
	: Scene{ pSceneManager,pUserResources }
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
 * @param[in] なし
 *
 * @return なし
 */
void StageSelectScene::Initialize()
{
	// ウィンドウサイズの取得
	SimpleMath::Vector2 windowSize = SimpleMath::Vector2(
		m_userResources->GetDeviceResources()->GetOutputSize().right,
		m_userResources->GetDeviceResources()->GetOutputSize().bottom
	);

	for (int i = 0; i < STAGES; i++)
	{
		SimpleMath::Vector2 pos = SimpleMath::Vector2(
			windowSize.x / static_cast<float>(STAGES) * i + 350.0f * 0.6f,
			windowSize.y * 0.5f
		);

		Tween::TweenData data =
		{
			Tween::UIParams{pos,SimpleMath::Vector2(1.0f,1.0f),0.0f,1.0f},
			Tween::UIParams{SimpleMath::Vector2(0.0f, 0.0f),SimpleMath::Vector2(0.05f,0.05f),0.0f,0.0f},
			0.5f,
			Tween::Ease::OutQuart,
			Tween::PlaybackMode::PingPong
		};

		std::unique_ptr<Button> panel = std::make_unique<Button>();
		panel->Initialize(m_userResources->GetResourceManager()->RequestPNG("stagePanel", L"Resources/Textures/UI/stagePanel.png"),
			data, SimpleMath::Vector2(350.f, 400.f),
			[this, i]() {
				std::string stageName = "Stage" + std::to_string(i + 1) + "Scene";
				ChangeScene(stageName);
			});
		m_stagePanels.push_back(std::move(panel));
	}

	m_numberBoard = std::make_unique<NumberRenderer2D>(
		SimpleMath::Vector2(48.f, 72.f),
		m_userResources->GetResourceManager()->RequestPNG("number", L"Resources/Textures/Text/number_48.png"),
		1);
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void StageSelectScene::Update(float elapsedTime)
{
	Keyboard::KeyboardStateTracker* kb = m_userResources->GetKeyboardTracker();

	// ステージ切り替え
	if (kb->pressed.D)
	{
		PanelReset(m_selectNum);

		m_selectNum++;
		if (m_selectNum == STAGES) m_selectNum = 0;
	}
	else if (kb->pressed.A)
	{
		PanelReset(m_selectNum);

		m_selectNum--;
		if (m_selectNum < 0) m_selectNum = STAGES - 1;
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
	if (kb->pressed.P)		// テスト用ステージへ
	{
		ChangeScene("TestScene");
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
void StageSelectScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"StageSelectScene");
	debugFont->AddString(0, 60, Colors::Yellow, L"Select:%d",m_selectNum);

	for (auto& panel : m_stagePanels)
	{
		panel->Draw(context);
	}

	// ウィンドウサイズの取得
	SimpleMath::Vector2 windowSize = SimpleMath::Vector2(
		m_userResources->GetDeviceResources()->GetOutputSize().right,
		m_userResources->GetDeviceResources()->GetOutputSize().bottom
	);

	// ステージ番号の描画
	for (int i = 0; i < STAGES; i++)
	{
		SimpleMath::Vector2 pos = SimpleMath::Vector2(
			windowSize.x / static_cast<float>(STAGES) * i + 380.0f * 0.6f,
			windowSize.y * 0.25f
		);
		m_numberBoard->SetNumber(i + 1);
		m_numberBoard->SetPosition(pos);
		m_numberBoard->Draw(context);
	}
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
	for (auto& panel : m_stagePanels)
	{
		panel->Finalize();
	}
	m_stagePanels.clear();
}

void StageSelectScene::PanelReset(int panelNum)
{
	m_stagePanels[panelNum]->Reset();
	m_stagePanels[panelNum]->Update(0.0f);
}
