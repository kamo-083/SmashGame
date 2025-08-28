/**
 * @file   TestScene.cpp
 *
 * @brief  ＸＸＸＸシーンに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TestScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/RenderContext.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] sceneManager    シーンを管理しているマネージャ
 * @param[in] resourceManager リソースを管理しているマネージャ
 */
TestScene::TestScene(SceneManager* pSceneManager, UserResources* pUserReources)
	: Scene{ pSceneManager,pUserReources }
	, m_keyMode{false}
{

}



/**
 * @brief デストラクタ
 */
TestScene::~TestScene()
{
	m_enemyManager->Finalize();
	m_effectManager->Finalize();
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TestScene::Initialize()
{
	// コリジョンマネージャーの作成
	m_collisionManager = std::make_unique<CollisionManager>();

	// レイヤーフィルターの登録
	auto& M = m_collisionManager->Matrix();
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyBody] = true;		// プレイヤーと敵
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::PlayerBody] = true;
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyBody] = true;		// 敵同士
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyAttack] = true;	// プレイヤーと敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::PlayerAttack] = false;	// プレイヤーとプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::PlayerAttack] = true;	// 敵とプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyAttack] = false;	// 敵と敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Stage] = true;			// プレイヤーとステージ
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Stage] = true;			// 敵とステージ
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Trigger] = true;			// プレイヤーとトリガー
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Trigger] = true;			// 敵とトリガー

	// カメラの作成
	m_camera = std::make_unique<Camera>();

	// エフェクトマネージャーの作成
	m_effectManager = std::make_unique<EffectManager>(m_userResorces->GetDeviceResources());
	m_effectManager->SetCamera(m_camera.get());

	// 武器UIの作成
	m_weaponUI = std::make_unique<WeaponUI>(m_userResorces->GetDeviceResources()->GetOutputSize().right,
		m_userResorces->GetDeviceResources()->GetOutputSize().bottom);
	m_weaponUI->Initialize(m_userResorces->GetResourceManager());

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_player->Initialize(m_userResorces->GetResourceManager(), m_collisionManager.get(),
						 &m_kbTracker, m_camera.get(), m_weaponUI.get(), &m_keyMode);
	//m_effectManager->CreateTrajectory(
	//	m_userResorces->GetResourceManager()->RequestTexture("smoke", L"Resources/Textures/Effect/smoke.png"),
	//	0.5f,
	//	2.0f,
	//	SimpleMath::Color(1, 1, 1, 1),
	//	&m_player->GetPosition(),
	//	false
	//);

	// 敵の作成
	m_enemyManager = std::make_unique<EnemyManager>(m_userResorces, m_collisionManager.get(), m_effectManager.get());
	EnemyManager::SpawnData data;
	data.type = EnemyManager::EnemyType::Ground;
	data.position = SimpleMath::Vector3(0.0f, 5.0f, -4.0f);
	m_enemyManager->Spawn(data);
	data.position = SimpleMath::Vector3(4.0f, 5.0f, -2.0f);
	m_enemyManager->Spawn(data);

	// 地面の作成
	m_grounds.push_back(std::make_unique<Ground>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext()));
	m_grounds.push_back(std::make_unique<Ground>(
		m_userResorces->GetDeviceResources()->GetD3DDeviceContext(), SimpleMath::Vector3(-5.0f, 1.5f, 0.0f), SimpleMath::Vector3(0.5f, 3.0f, 5.0f)));
	m_grounds.push_back(std::make_unique<Ground>(
		m_userResorces->GetDeviceResources()->GetD3DDeviceContext(), SimpleMath::Vector3(0.0f, 1.5f, -5.0f), SimpleMath::Vector3(5.0f, 3.0f, 0.5f)));
	for (std::unique_ptr<Ground>& ground : m_grounds)
	{
		ground->Initialize(m_collisionManager.get());
	}

	// ゴールの作成
	m_goal = std::make_unique<Goal>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_goal->Initialize(SimpleMath::Vector3(-2.0f, 1.0f, 2.0f));

	// エリアの作成
	m_area = std::make_unique<CountArea>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_area->Initialize(m_collisionManager.get(), SimpleMath::Vector3(0.0f, 1.0f, 3.0f), 1.0f, 1.0f, CountArea::TriggerMode::AllOut);

	// 箱の作成
	m_bounceBox = std::make_unique<BounceBox>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_bounceBox->Initialize(SimpleMath::Vector3(2.0f, 0.5f, 2.0f));

	// 的の作成
	m_targetBox = std::make_unique<TargetBox>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_targetBox->Initialize(m_goal.get(), SimpleMath::Vector3(-2.0f, 0.5f, -2.0f));

	// カメラの初期化
	m_camera->Initialize(&m_player->GetPosition());

	// キー操作のモードの初期化
	m_keyMode = true;
}



/**
 * @brief 更新処理
 *
 * @param[in] inputDevice 入力デバイス
 *
 * @return なし
 */
void TestScene::Update(float elapsedTime)
{
	// キーボードの更新
	auto kb = Keyboard::Get().GetState();
	m_kbTracker.Update(kb);

	// キー操作のモード
	if (m_kbTracker.pressed.K) m_keyMode = !m_keyMode;

	// プレイヤーの更新
	m_player->Update(elapsedTime);

	// 敵の更新
	m_enemyManager->Update(elapsedTime, m_player.get());

	// 箱の更新
	m_bounceBox->Update(elapsedTime);

	// エリアの更新
	m_area->Update();

	// カメラの更新
	if(!m_keyMode) m_camera->Rotation(&m_kbTracker);
	m_camera->Update(&m_kbTracker, elapsedTime);

	// エフェクトの更新
	m_effectManager->Update(elapsedTime);

	// 当たり判定の更新
	m_collisionManager->Update(elapsedTime);

	// 仮リスポーン
	if (m_player->GetPosition().y <= -10.0f)
		m_player->Initialize(m_userResorces->GetResourceManager(), m_collisionManager.get(),
							 &m_kbTracker, m_camera.get(), m_weaponUI.get(), &m_keyMode);

	// 当たり判定の処理
	// 地面
	for (std::unique_ptr<Ground>& ground : m_grounds)
	{
		m_bounceBox->DetectCollisionToBox(ground->GetCollider());
	}
	// プレイヤー
	if (m_player->GetIsAttack())
	{
		m_bounceBox->DetectCollisionToAttack(*m_player->GetAttackCollider(), *m_player->GetCollider(), m_player->GetAttackForce());
	}
	// ゴール
	m_goal->DetectCollisionToPlayer(*m_player->GetCollider());

	// シーンの切り替え
	if (m_kbTracker.pressed.P)
	{
		m_effectManager->Finalize();
		ChangeScene("TitleScene");
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TestScene::Render(RenderContext context, Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 30, Colors::White, L"TestScene");

	context.view = m_camera->GetView();

	// プレイヤーの描画
	m_player->Draw(context, debugFont);

	// 敵の描画
	m_enemyManager->Draw(context);

	// 地面の描画
	for (std::unique_ptr<Ground>& ground : m_grounds)
	{
		ground->Draw(context);
	}

	// 箱の描画
	m_bounceBox->Draw(context, debugFont);

	// 的の描画
	m_targetBox->Draw(context);

	// ゴールの描画
	m_goal->Draw(context,debugFont);

	// エリアの描画
	m_area->Draw(context, debugFont);

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// 武器UIの描画
	m_weaponUI->Draw(context.spriteBatch);

	// エフェクトの描画
	m_effectManager->Draw(context.projection);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TestScene::Finalize()
{
	if (m_player) m_player->Finalize();
	if (m_enemyManager) m_enemyManager->Finalize();
	if (m_effectManager)	m_effectManager->Finalize();
	if (m_bounceBox)	m_bounceBox->Finalize();
	if (m_targetBox)	m_targetBox->Finalize();
	if (m_goal)	m_goal->Finalize();

	for (auto& ground : m_grounds)
	{
		ground->Finalize();
	}
}
