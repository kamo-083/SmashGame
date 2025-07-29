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
TestScene::TestScene(SceneManager* pSceneManager, ResourceManager* pResourceManager)
	: Scene{ pSceneManager,pResourceManager }
	, m_keyMode{false}
{

}



/**
 * @brief デストラクタ
 */
TestScene::~TestScene()
{

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
	// カメラの作成
	m_camera = std::make_unique<Camera>();

	// 武器UIの作成
	m_weaponUI = std::make_unique<WeaponUI>(m_sceneManager->GetDeviceResources()->GetOutputSize().right,
		m_sceneManager->GetDeviceResources()->GetOutputSize().bottom);
	m_weaponUI->Initialize(m_resourceManager);

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
	m_player->Initialize(m_resourceManager, &m_kbTracker, m_camera.get(), m_weaponUI.get(), &m_keyMode);

	// 敵の作成
	m_enemy = std::make_unique<GroundEnemy>();
	m_enemy->Initialize(m_resourceManager);

	// 地面の作成
	m_ground = std::make_unique<Ground>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
	m_ground->Initialize();

	// ゴールの作成
	m_goal = std::make_unique<Goal>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
	m_goal->Initialize(SimpleMath::Vector3(-2.0f, 1.0f, 2.0f));

	// 箱の作成
	m_bounceBox = std::make_unique<BounceBox>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
	m_bounceBox->Initialize(SimpleMath::Vector3(2.0f, 0.5f, 2.0f));
	
	// 的の作成
	m_targetBox = std::make_unique<TargetBox>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
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
	m_enemy->CalculatePlayerRelationData(m_player->GetPosition(), m_player->GetRadius());
	m_enemy->Update(elapsedTime);

	// 箱の更新
	m_bounceBox->Update(elapsedTime);

	// カメラの更新
	if(!m_keyMode) m_camera->Rotation(&m_kbTracker);
	m_camera->Update(&m_kbTracker, elapsedTime);

	// 当たり判定の処理
	// プレイヤー
	m_player->DetectCollisionToBox(m_ground->GetCollider());
	m_player->DetectCollisionToBox(m_bounceBox->GetCollider());
	m_player->DetectCollisionToSphere(*m_enemy->GetCollider());
	// 敵
	m_enemy->DetectCollisionToBox(m_ground->GetCollider());
	m_enemy->DetectCollisionToAttack(*m_player->GetAttackCollider(), m_player->GetAttackForce());
	// 箱
	if (m_player->GetIsAttack())
	{
		m_bounceBox->DetectCollisionToAttack(*m_player->GetAttackCollider(), m_player->GetAttackForce());
	}
	m_bounceBox->DetectCollisionToBox(m_ground->GetCollider());
	// 的
	m_targetBox->DetectCollisionToEnemy(*m_enemy->GetCollider(), 1.0f);
	// ゴール
	m_goal->DetectCollisionToPlayer(*m_player->GetCollider());

	// シーンの切り替え
	if (m_kbTracker.pressed.P)
	{
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
	m_enemy->Draw(context, debugFont);

	// 地面の描画
	m_ground->Draw(context);

	// 箱の描画
	m_bounceBox->Draw(context);

	// 的の描画
	m_targetBox->Draw(context);

	// ゴールの描画
	m_goal->Draw(context,debugFont);

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// 武器UIの描画
	m_weaponUI->Draw(context.spriteBatch);
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
	
}
