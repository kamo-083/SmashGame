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
	//カメラの作成
	m_camera = std::make_unique<Camera>();

	//プレイヤーの作成
	m_player = std::make_unique<Player>();
	m_player->Initialize(m_resourceManager, &m_kbTracker, m_camera.get());

	// 敵の作成
	m_enemy = std::make_unique<GroundEnemy>();
	m_enemy->Initialize(m_resourceManager);

	//地面の作成
	m_ground = std::make_unique<Ground>(m_sceneManager->GetDeviceResources()->GetD3DDeviceContext());
	m_ground->Initialize();

	// キー操作のモードの初期化
	m_keyMode = false;
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

	// カメラの更新
	if(m_keyMode) m_camera->Rotation(&m_kbTracker);
	m_camera->Update(&m_kbTracker, elapsedTime);

	// 当たり判定の処理
	// プレイヤー
	m_player->DetectCollisionToBox(m_ground->GetCollider());
	m_player->DetectCollisionToSphere(*m_enemy->GetCollider());
	// 敵
	m_enemy->DetectCollisionToBox(m_ground->GetCollider());

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
