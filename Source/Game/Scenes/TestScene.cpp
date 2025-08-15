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
	m_weaponUI = std::make_unique<WeaponUI>(m_userResorces->GetDeviceResources()->GetOutputSize().right,
		m_userResorces->GetDeviceResources()->GetOutputSize().bottom);
	m_weaponUI->Initialize(m_userResorces->GetResourceManager());

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_player->Initialize(m_userResorces->GetResourceManager(), &m_kbTracker, m_camera.get(), m_weaponUI.get(), &m_keyMode);

	// 敵の作成
	m_enemy = std::make_unique<EnemyManager>(m_userResorces);
	EnemyManager::SpawnData data;
	data.type = EnemyManager::EnemyType::Ground;
	data.position = SimpleMath::Vector3(0.0f, 5.0f, -4.0f);
	m_enemy->Spawn(data);
	data.position = SimpleMath::Vector3(4.0f, 5.0f, -2.0f);
	m_enemy->Spawn(data);

	// 地面の作成
	m_grounds.push_back(std::make_unique<Ground>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext()));
	m_grounds.push_back(std::make_unique<Ground>(
		m_userResorces->GetDeviceResources()->GetD3DDeviceContext(), SimpleMath::Vector3(-5.0f, 1.5f, 0.0f), SimpleMath::Vector3(0.5f, 3.0f, 5.0f)));
	m_grounds.push_back(std::make_unique<Ground>(
		m_userResorces->GetDeviceResources()->GetD3DDeviceContext(), SimpleMath::Vector3(0.0f, 1.5f, -5.0f), SimpleMath::Vector3(5.0f, 3.0f, 0.5f)));
	for (std::unique_ptr<Ground>& ground : m_grounds)
	{
		ground->Initialize();
	}

	// ゴールの作成
	m_goal = std::make_unique<Goal>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_goal->Initialize(SimpleMath::Vector3(-2.0f, 1.0f, 2.0f));

	// 箱の作成
	m_bounceBox = std::make_unique<BounceBox>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_bounceBox->Initialize(SimpleMath::Vector3(2.0f, 0.5f, 2.0f));

	// 的の作成
	m_targetBox = std::make_unique<TargetBox>(m_userResorces->GetDeviceResources()->GetD3DDeviceContext());
	m_targetBox->Initialize(m_goal.get(), SimpleMath::Vector3(-2.0f, 0.5f, -2.0f));

	// カメラの初期化
	m_camera->Initialize(&m_player->GetPosition());

	// エフェクトマネージャーの設定
	m_userResorces->GetEffectManager()->SetCamera(m_camera.get());

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
	m_enemy->Update(elapsedTime, m_player.get());

	// 箱の更新
	m_bounceBox->Update(elapsedTime);

	// カメラの更新
	if(!m_keyMode) m_camera->Rotation(&m_kbTracker);
	m_camera->Update(&m_kbTracker, elapsedTime);

	// エフェクトの更新
	m_userResorces->GetEffectManager()->Update(elapsedTime);

	// 仮リスポーン
	if (m_player->GetPosition().y <= -10.0f)
		m_player->Initialize(m_userResorces->GetResourceManager(), &m_kbTracker, m_camera.get(), m_weaponUI.get(), &m_keyMode);
	//if (m_enemy->GetPosition().y <= -30.0f)
	//	m_enemy->Initialize(m_userResorces->GetResourceManager(), { 0.0f, 5.0f, -4.0f });

	// 当たり判定の処理
	// 地面
	for (std::unique_ptr<Ground>& ground : m_grounds)
	{
		m_player->DetectCollisionToBox(ground->GetCollider());
		//m_enemy->DetectCollisionToBox(ground->GetCollider());
		m_bounceBox->DetectCollisionToBox(ground->GetCollider());
	}
	// プレイヤー
	m_player->DetectCollisionToBox(m_bounceBox->GetCollider());
	//m_player->DetectCollisionToSphere(*m_enemy->GetCollider());
	//if (m_enemy->GetIsAttack()) m_player->DetectCollisionToAttack(*m_enemy->GetAttackCollider(), m_enemy->GetAttackForce());
	// 敵
	//if (m_player->GetIsAttack()) m_enemy->DetectCollisionToAttack(*m_player->GetAttackCollider(), m_player->GetAttackForce());
	// 箱
	if (m_player->GetIsAttack())
	{
		m_bounceBox->DetectCollisionToAttack(*m_player->GetAttackCollider(), *m_player->GetCollider(), m_player->GetAttackForce());
	}
	// 的
	//m_targetBox->DetectCollisionToEnemy(*m_enemy->GetCollider(), m_enemy->GetStateType());
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
	m_enemy->Draw(context);

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

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// 武器UIの描画
	m_weaponUI->Draw(context.spriteBatch);

	// エフェクトの描画
	m_userResorces->GetEffectManager()->Draw(context.projection);
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
