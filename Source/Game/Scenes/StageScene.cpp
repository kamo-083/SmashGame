/**
 * @file   StageScene.cpp
 *
 * @brief  ステージシーンに関するソースファイル
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/RenderContext.h"

#include"Source/Game/Common/RenderTexture.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param sceneManager    シーンを管理しているマネージャ
 * @param resourceManager リソースを管理しているマネージャ
 */
StageScene::StageScene(
	SceneManager* pSceneManager, UserResources* pUserResources,
	std::string path, ClearConditionsUI::ConditionsType clearCondition)
	: Scene{ pSceneManager,pUserResources }
	, m_keyMode{ false }
	, m_stageFilePath{ path }
	, m_overlayMode{ Overlay::NONE }
	, CLEAR_CONDITIONS{ clearCondition }
{

}



/**
 * @brief デストラクタ
 */
StageScene::~StageScene()
{

}



/**
 * @brief 初期化処理
 *
 * @para なし
 *
 * @return なし
 */
void StageScene::Initialize()
{
	// 当たり判定マネージャーの作成
	m_collisionManager = std::make_unique<CollisionManager>();

	// レイヤーフィルターの登録
	auto& M = m_collisionManager->GetLayerMatrix();
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyBody] = true;		// プレイヤーと敵
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyBody] = true;		// 敵同士
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyAttack] = true;	// プレイヤーと敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::PlayerAttack] = false;	// プレイヤーとプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::PlayerAttack] = true;	// 敵とプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyAttack] = false;	// 敵と敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Stage] = true;			// プレイヤーとステージ
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Stage] = true;			// 敵とステージ
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Trigger] = true;		// プレイヤーとトリガー
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Trigger] = true;		// 敵とトリガー

	// カメラの作成
	m_camera = std::make_unique<Camera>();

	// エフェクトマネージャーの作成
	m_effectManager = std::make_unique<EffectManager>(m_userResources->GetDeviceResources(),m_userResources->GetStates());
	m_effectManager->LoadShaders(m_userResources->GetShaderManager());
	m_effectManager->SetCamera(m_camera.get());

	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		m_userResources->GetDeviceResources()->GetOutputSize().right,
		m_userResources->GetDeviceResources()->GetOutputSize().bottom
	);

	// リソースマネージャーのポインタを取得
	ResourceManager* pRM = m_userResources->GetResourceManager();

	// 攻撃UIの作成
	m_attackUI = std::make_unique<AttackUI>(m_userResources->GetDeviceResources()->GetOutputSize().right,
											m_userResources->GetDeviceResources()->GetOutputSize().bottom);
	m_attackUI->Initialize(pRM);

	// リザルトUIの作成
	m_resultUI = std::make_unique<StageResultUI>();
	m_resultUI->Initialize(
		pRM->RequestPNG("resultPanel", L"Resources/Textures/UI/resultPanel.png"),
		DirectX::SimpleMath::Vector2(350.f, 400.f), windowSize);

	// ステージクリア条件UIの作成
	m_conditionsUI = std::make_unique<ClearConditionsUI>(CLEAR_CONDITIONS);
	m_conditionsUI->Initialize(windowSize, pRM);

	// 操作方法UIの画像読み込み
	OperationUI::Textures uiTextures;
	uiTextures.nomalArrow = pRM->RequestPNG("arrow", L"Resources/Textures/UI/arrow_triangle.png");
	uiTextures.rotateArrow = pRM->RequestPNG("rotate", L"Resources/Textures/UI/arrow_rotate.png");
	uiTextures.keyText = pRM->RequestPNG("box", L"Resources/Textures/text/operationText.png");
	uiTextures.icon = pRM->RequestPNG("camera", L"Resources/Textures/UI/camera.png");

	// カメラ操作UIの作成
	m_cameraUI = std::make_unique<OperationUI>();
	m_cameraUI->Initialize(uiTextures, DirectX::SimpleMath::Vector2(250.f, 600.f), 350.f, false, DirectX::SimpleMath::Vector2(200.f, 135.f));

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_userResources, m_effectManager.get(), this);
	m_player->Initialize(pRM, m_collisionManager.get(),
						 m_userResources->GetKeyboardTracker(), m_camera.get(), m_attackUI.get(), &m_keyMode);

	// 敵マネージャーの作成
	m_enemyManager = std::make_unique<EnemyManager>(m_userResources, m_collisionManager.get(), m_effectManager.get());
	m_enemyManager->Initialize();

	// ステージマネージャーの作成
	m_stageManager = std::make_unique<StageManager>(this);
	m_stageManager->CreateStage(m_userResources, m_collisionManager.get(), m_enemyManager.get(),
								m_stageFilePath);

	// カメラの初期化
	m_camera->Initialize(&m_player->GetPosition());

	// スカイドームの作成
	m_sky = std::make_unique<Sky>();
	m_sky->Initialize(m_userResources);
	m_sky->SetPosition(&m_player->GetPosition());

	// キー操作のモードの初期化
	m_keyMode = true;

	// 表示状態の初期化
	m_overlayMode = Overlay::NONE;

	// BGM・SEの読み込み
	AudioManager* pAM = m_userResources->GetAudioManager();
	pAM->LoadMP3("stageBGM", "Resources/Sounds/BGM/iwashiro_orange_hill.mp3");
	pAM->LoadMP3("startSE", "Resources/Sounds/SE/fue.mp3");
	pAM->LoadMP3("canGoalSE", "Resources/Sounds/SE/bell.mp3");
	pAM->LoadMP3("clearSE", "Resources/Sounds/SE/one08.mp3");
	pAM->LoadMP3("attackSE", "Resources/Sounds/SE/hit01.mp3");

	// BGMの再生
	pAM->Play("stageBGM", true);

	// スタートSEの再生
	PlaySE("startSE");
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void StageScene::Update(float elapsedTime)
{
	if (m_overlayMode == Overlay::RESULT)
	{
		m_resultUI->Update(elapsedTime);

		// シーンの切り替え
		if (m_userResources->GetKeyboardTracker()->pressed.Space)
		{
			// BGMの停止
			m_userResources->GetAudioManager()->Stop("stageBGM");

			ChangeScene("StageSelectScene");
		}

		return;
	}

	// キー操作のモード切り替え
	if (m_userResources->GetKeyboardTracker()->pressed.K)
	{
		m_keyMode = !m_keyMode;

		// UI切り替え
		m_cameraUI->Active(!m_cameraUI->IsActive());
		m_attackUI->SwitchUIMode();

		// SEの再生
		PlaySE("cursorSE");
	}

	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemyManager->Update(elapsedTime, m_player.get());

	// カメラの更新
	if (!m_keyMode) m_camera->Rotation(m_userResources->GetKeyboardTracker());
	m_camera->Update(m_userResources->GetKeyboardTracker(), elapsedTime);

	// ステージの更新
	m_stageManager->Update(elapsedTime, m_camera->GetEye(), m_camera->GetUp());

	// エフェクトの更新
	m_effectManager->Update(elapsedTime);

	// UIの更新
	m_conditionsUI->Update(elapsedTime);	// クリア条件
	m_attackUI->Update(elapsedTime);		// 攻撃
	m_cameraUI->Update(elapsedTime);		// カメラ

	// 当たり判定の更新
	m_collisionManager->Update(elapsedTime);

	// 落下時のリスポーン
	if (m_player->GetPosition().y <= m_player->GetKillHeight()) m_player->Respawn();

	// リザルトの表示　
	if (m_stageManager->IsGoal())
	{
		// SEの再生
		PlaySE("clearSE");

		m_overlayMode = Overlay::RESULT;
	}

	// ステージ選択へ戻る
	if (m_userResources->GetKeyboardTracker()->pressed.P)
	{
		// BGMの停止
		m_userResources->GetAudioManager()->Stop("stageBGM");

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
void StageScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageScene");

	context.view = m_camera->GetView();

	// スカイドームの描画
	m_sky->Draw(context);

	// プレイヤーの描画
	m_player->Draw(context, debugFont);

	// 敵の描画
	m_enemyManager->Draw(context);

	// ステージの描画
	m_stageManager->Draw(context, debugFont);

	// エフェクトの描画
	m_effectManager->Draw(context.proj);

	// ステージの描画(半透明オブジェクト)
	m_stageManager->DrawTranslucent(context, debugFont);

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// UIの描画
	m_conditionsUI->Draw(context);	// クリア条件
	m_attackUI->Draw(context);	  	// 攻撃
	m_cameraUI->Draw(context);	  	// カメラ

	if (m_overlayMode == Overlay::RESULT)
	{
		m_resultUI->Draw(context);
	}
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::Finalize()
{
	if (m_player) m_player->Finalize();
	m_player.reset();

	if (m_conditionsUI) m_conditionsUI->Finalize();
	m_conditionsUI.reset();

	if (m_attackUI) m_attackUI->Finalize();
	m_attackUI.reset();

	if (m_resultUI) m_resultUI->Finalize();
	m_resultUI.reset();

	if (m_cameraUI) m_cameraUI->Finalize();
	m_cameraUI.reset();

	if (m_sky) m_sky->Finalize();
	m_sky.reset();

	if (m_enemyManager) m_enemyManager->Finalize();
	m_enemyManager.reset();

	if (m_stageManager) m_stageManager->Finalize();
	m_stageManager.reset();

	if (m_effectManager) m_effectManager->Finalize();
	m_effectManager.reset();

	m_camera.reset();
	m_collisionManager.reset();
}



/**
 * @brief SEの再生
 *
 * @param seName 再生するSEの名前
 *
 * @return なし
 */
void StageScene::PlaySE(std::string seName)
{
	m_userResources->GetAudioManager()->Play(seName, false);
}
