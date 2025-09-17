/**
 * @file   StageScene.cpp
 *
 * @brief  ステージシーンに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/RenderContext.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] sceneManager    シーンを管理しているマネージャ
 * @param[in] resourceManager リソースを管理しているマネージャ
 */
StageScene::StageScene(SceneManager* pSceneManager, UserResources* pUserResources, std::string path)
	: Scene{ pSceneManager,pUserResources }
	, m_keyMode{ false }
	, m_stageFilePath{ path }
	, m_overlayMode{ Overlay::NONE }
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
 * @param[in] なし
 *
 * @return なし
 */
void StageScene::Initialize()
{
	// コリジョンマネージャーの作成
	m_collisionManager = std::make_unique<CollisionManager>();

	// レイヤーフィルターの登録
	auto& M = m_collisionManager->Matrix();
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
	m_effectManager->SetCamera(m_camera.get());

	// ウィンドウサイズの取得
	SimpleMath::Vector2 windowSize = SimpleMath::Vector2(
		m_userResources->GetDeviceResources()->GetOutputSize().right,
		m_userResources->GetDeviceResources()->GetOutputSize().bottom
	);

	// 武器UIの作成
	m_weaponUI = std::make_unique<WeaponUI>(m_userResources->GetDeviceResources()->GetOutputSize().right,
											m_userResources->GetDeviceResources()->GetOutputSize().bottom);
	m_weaponUI->Initialize(m_userResources->GetResourceManager());

	// リザルトUIの作成
	m_resultUI = std::make_unique<StageResultUI>();
	m_resultUI->Initialize(m_userResources->GetResourceManager()->RequestTexture("resultPanel", L"Resources/Textures/UI/resultPanel.png"),
						   SimpleMath::Vector2(350.f, 400.f), windowSize);

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_userResources, m_effectManager.get());
	m_player->Initialize(m_userResources->GetResourceManager(), m_collisionManager.get(),
						 m_userResources->GetKeyboardTracker(), m_camera.get(), m_weaponUI.get(), &m_keyMode);

	// エネミーマネージャーの作成
	m_enemyManager = std::make_unique<EnemyManager>(m_userResources, m_collisionManager.get(), m_effectManager.get());
	m_enemyManager->Initialize();

	// ステージマネージャーの作成
	m_stageManager = std::make_unique<StageManager>();
	m_stageManager->CreateStage(m_userResources, m_collisionManager.get(), m_enemyManager.get(),
								m_stageFilePath);

	// カメラの初期化
	m_camera->Initialize(&m_player->GetPosition());

	// キー操作のモードの初期化
	m_keyMode = true;

	// 表示状態の初期化
	m_overlayMode = Overlay::NONE;
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
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
			m_effectManager->Finalize();
			ChangeScene("StageSelectScene");
		}

		return;
	}

	// キー操作のモード切り替え
	if (m_userResources->GetKeyboardTracker()->pressed.K) m_keyMode = !m_keyMode;

	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemyManager->Update(elapsedTime, m_player.get());

	// カメラの更新
	if (!m_keyMode) m_camera->Rotation(m_userResources->GetKeyboardTracker());
	m_camera->Update(m_userResources->GetKeyboardTracker(), elapsedTime);

	// ステージマネージャーの更新
	m_stageManager->Update(elapsedTime);

	// エフェクトの更新
	m_effectManager->Update(elapsedTime);

	// UIの更新
	m_weaponUI->Update(elapsedTime);

	// 当たり判定の更新
	m_collisionManager->Update(elapsedTime);

	// 落下時のリスポーン
	if (m_player->GetPosition().y <= m_player->GetKillHeight()) m_player->Respawn();

	// リザルトの表示　
	if (m_stageManager->IsGoal() || m_userResources->GetKeyboardTracker()->pressed.P)
	{
		m_overlayMode = Overlay::RESULT;
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
void StageScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"StageScene");

	context.view = m_camera->GetView();

	// プレイヤーの描画
	m_player->Draw(context, debugFont);

	// 敵の描画
	m_enemyManager->Draw(context);

	// ステージマネージャーの描画
	m_stageManager->Draw(context, debugFont);

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// 武器UIの描画
	m_weaponUI->Draw(context);

	// エフェクトの描画
	m_effectManager->Draw(context.projection);

	if (m_overlayMode == Overlay::RESULT)
	{
		m_resultUI->Draw(context);
	}
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StageScene::Finalize()
{
	if (m_player) m_player->Finalize();
	m_player.reset();

	if (m_weaponUI) m_weaponUI->Finalize();
	m_weaponUI.reset();

	if (m_resultUI) m_resultUI->Finalize();
	m_resultUI.reset();

	if (m_enemyManager) m_enemyManager->Finalize();
	m_enemyManager.reset();

	if (m_stageManager) m_stageManager->Finalize();
	m_stageManager.reset();

	if (m_effectManager) m_effectManager->Finalize();
	m_effectManager.reset();

	m_camera.reset();
	m_weaponUI.reset();
	m_collisionManager.reset();
}
