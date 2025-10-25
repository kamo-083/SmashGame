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
#include "Source/Game/Common/CollisionManager.h"
#include "Source/Game/Effect/EffectManager.h"
#include "Source/Game/GameObjects/Camera.h"
#include "Source/Game/GameObjects/Player/Player.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/GameObjects/Stage/StageManager.h"
#include "Source/Game/GameObjects/Stage/Objects/Ground.h"
#include "Source/Game/GameObjects/Sky.h"
#include "Source/Game/UI/UIWidget.h"
#include "Source/Game/UI/AttackUI.h"
#include "Source/Game/UI/StageResultUI.h"
#include "Source/Game/UI/OperationUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pSM			  シーンを管理しているマネージャ
 * @param pUR			  リソースを管理しているマネージャ
 * @param path			  ステージデータのファイルパス
 * @param clearCondition  クリア条件
 */
StageScene::StageScene(
	SceneManager* pSM, UserResources* pUR,
	std::string path, ClearConditionsUI::ConditionsType clearCondition)
	: Scene{ pSM,pUR }
	, m_keyMode{ false }
	, m_stageFilePath{ path }
	, m_overlayMode{ Overlay::NONE }
	, CLEAR_CONDITIONS{ clearCondition }
{
	DX::DeviceResources* pDR = m_userResources->GetDeviceResources();

	// プリミティブバッチの作成
	ID3D11DeviceContext* context = pDR->GetD3DDeviceContext();
	m_primitiveBatch = std::make_unique<DirectX::DX11::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);

	// ベーシックエフェクトの作成
	ID3D11Device* device = pDR->GetD3DDevice();
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device); 
	m_basicEffect->SetLightingEnabled(false);
	m_basicEffect->SetVertexColorEnabled(false);
	m_basicEffect->SetTextureEnabled(true);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);
}



/**
 * @brief デストラクタ
 */
StageScene::~StageScene()
{
	m_primitiveBatch.reset();
	m_basicEffect.reset();
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
	// よく使うポインタを取得
	DX::DeviceResources* pDR = m_userResources->GetDeviceResources();	// デバイスリソース
	ResourceManager* pRM = m_userResources->GetResourceManager();		// リソースマネージャー
	AudioManager* pAM = m_userResources->GetAudioManager();				// オーディオマネージャー

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
	m_effectManager = std::make_unique<EffectManager>(pDR, m_userResources->GetStates());
	m_effectManager->LoadShaders(m_userResources->GetShaderManager());
	m_effectManager->SetCamera(m_camera.get());

	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		static_cast<float>(pDR->GetOutputSize().right),
		static_cast<float>(pDR->GetOutputSize().bottom)
	);

	// 操作方法UIの画像読み込み
	OperationUI::Textures opTextures;
	opTextures.nomalArrow = pRM->RequestPNG("arrow", L"Resources/Textures/UI/arrow_triangle.png");
	opTextures.rotateArrow = pRM->RequestPNG("rotate", L"Resources/Textures/UI/arrow_rotate.png");
	opTextures.keyText = pRM->RequestPNG("box", L"Resources/Textures/text/operationText.png");

	// 操作方法UIの引数用構造体作成
	OperationUI::OperationUIDesc opUIDesc =
	{
		opTextures,
		ARROW_SIZE_DEFAULT,
		ARROW_SIZE_ROTATE,
		TEXT_UV_LEFT,
		TEXT_SIZE,
		CAMERA_ICON_SIZE
	};

	// 攻撃変更UIの画像読み込み・引数用構造体作成
	AttackUI::AttackUIDesc atkUIDesc =
	{
		pRM->RequestPNG("attack_basic", L"Resources/Textures/UI/basicAtk.png"),
		pRM->RequestPNG("attack_rolling", L"Resources/Textures/UI/rollingAtk.png"),
		pRM->RequestPNG("attack_heavy", L"Resources/Textures/UI/heavyAtk.png"),
		ATTACK_ICON_SIZE.x, ATTACK_ICON_SIZE.y
	};

	// 攻撃UIの作成
	m_attackUI = std::make_unique<AttackUI>(
		windowSize.x, windowSize.y);
	m_attackUI->Initialize(atkUIDesc, opUIDesc);

	// カメラ操作UIの作成
	opTextures.icon = pRM->RequestPNG("camera", L"Resources/Textures/UI/camera.png");	// アイコン画像追加
	opUIDesc.textures = opTextures;														// 再設定
	m_cameraUI = std::make_unique<OperationUI>();
	m_cameraUI->Initialize(opUIDesc, CAMERA_UI_POS, CAMERA_UI_ARROW_INTERVAL, false);

	// リザルトUIの作成
	m_resultUI = std::make_unique<StageResultUI>();
	m_resultUI->Initialize(
		pRM->RequestPNG("resultPanel", L"Resources/Textures/UI/resultPanel.png"),
		RESULT_WINDOW_SIZE, windowSize);

	// ステージクリア条件UIの作成
	m_conditionsUI = std::make_unique<ClearConditionsUI>(CLEAR_CONDITIONS);
	m_conditionsUI->Initialize(
		windowSize,
		pRM->RequestPNG("conditionsText", L"Resources/Textures/Text/conditionsText.png"),
		CONDITIONS_TEXT_SIZE
	);

	// 深度ステンシルステートの作成
	CreateDepthStencilState(pDR->GetD3DDevice());

	// プレイヤーの作成
	m_player = std::make_unique<Player>(m_userResources, m_effectManager.get(), this);
	m_player->Initialize(pRM, m_collisionManager.get(),
		m_userResources->GetKeyboardTracker(), m_camera.get(), m_attackUI.get(), &m_keyMode);

	// 敵マネージャーの作成
	m_enemyManager = std::make_unique<EnemyManager>(
		m_userResources, m_collisionManager.get(), m_effectManager.get());
	m_enemyManager->Initialize();

	// ステージマネージャーの作成
	m_stageManager = std::make_unique<StageManager>(this, m_depthStencilState_stage.Get());
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

	// テクスチャの読み込み
	m_textures = std::make_unique<Textures>();
	m_textures->shadow = pRM->RequestDDS("shadow", L"Resources/Textures/Others/shadow.dds");

	// BGM・SEの読み込み
	pAM->LoadMP3("stageBGM", "Resources/Sounds/BGM/iwashiro_orange_hill.mp3");
	pAM->LoadMP3("startSE", "Resources/Sounds/SE/fue.mp3");
	pAM->LoadMP3("canGoalSE", "Resources/Sounds/SE/bell.mp3");
	pAM->LoadMP3("clearSE", "Resources/Sounds/SE/one08.mp3");
	pAM->LoadMP3("attackSE", "Resources/Sounds/SE/hit01.mp3");

	// BGM・SEの音量変更
	pAM->SetVolume("stageBGM", BGM_VOLUME);

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
	// リザルト表示中
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
	if (m_userResources->GetKeyboardTracker()->pressed.X)
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
	m_camera->Update(elapsedTime);

	// ステージの更新
	m_stageManager->Update(elapsedTime, m_camera->GetEye(), m_camera->GetUp());

	// エフェクトの更新
	m_effectManager->Update(elapsedTime);

	// UIの更新
	m_conditionsUI->Update(elapsedTime);	// クリア条件
	m_attackUI->Update(elapsedTime);		// 攻撃
	m_cameraUI->Update(elapsedTime);		// カメラ

	// 当たり判定の更新
	m_collisionManager->Update();

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
	if (m_userResources->GetKeyboardTracker()->pressed.Q)
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
void StageScene::Render(RenderContext context, DebugFont* debugFont)
{
	// デバッグ情報の追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageScene");

	// ビュー行列の反映
	context.view = m_camera->GetView();

	// スカイドームの描画
	m_sky->Draw(context);

	// プレイヤーの描画
	m_player->Draw(context, debugFont);

	// 敵の描画
	m_enemyManager->Draw(context);

	// ステージの描画
	m_stageManager->Draw(context, debugFont);

	// 影の描画
	SettingShadow(context);
	float groundHeight = m_stageManager->GetGround(0)->GetHeight().y;
	m_primitiveBatch->Begin();

	// プレイヤー
	DrawShadow(
		DirectX::SimpleMath::Vector3(m_player->GetPosition().x, groundHeight, m_player->GetPosition().z),
		m_player->GetRadius());
	
	// 敵
	for (int i = 1; i <= m_enemyManager->GetEnemyNum(); i++)
	{
		// 対象が存在しなかったら飛ばす
		if (!m_enemyManager->GetEnemyByID(i)) continue;

		// 座標を取得
		DirectX::SimpleMath::Vector3 enemyPos = m_enemyManager->GetEnemyByID(i)->GetPosition();
		enemyPos.y = groundHeight;
		// 半径を取得
		float enemyRadius = m_enemyManager->GetEnemyByID(i)->GetRadius();
		// 影を描画
		DrawShadow(enemyPos, enemyRadius);
	}

	m_primitiveBatch->End();

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

	// リザルトの描画
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



/**
 * @brief 深度ステンシルステートの作成
 *
 * @param device	デバイスのポインタ
 *
 * @return なし
 */
void StageScene::CreateDepthStencilState(ID3D11Device* device)
{
	// 深度ステンシルステートの作成
	D3D11_DEPTH_STENCIL_DESC desc = {};	// デフォルトで初期化

	// ステージ用
	// 深度テスト
	desc.DepthEnable = TRUE;									// 深度テストをする
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// 深度バッファに書き込みをする
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				// 深度値以下なら更新する
	// ステンシルテスト
	desc.StencilEnable = TRUE;									// ステンシルテストをする 
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 初期値(0xff)
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 初期値(0xff)
	// 表面
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		// テスト結果が0ならば
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;	// 1にする(表示する)
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// そのまま
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// そのまま
	// 裏面
	desc.BackFace = desc.FrontFace;								// 裏面も表面と同じ
	// 深度ステンシルステートの作成
	device->CreateDepthStencilState(&desc, m_depthStencilState_stage.ReleaseAndGetAddressOf());

	// キャラクターの影用
	// 深度テスト
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// 深度バッファに書き込みしない
	// 表面
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		// 1にする(表示する)
	// 裏面
	desc.BackFace = desc.FrontFace;								// 裏面も表面と同じ
	// 深度ステンシルステートの作成
	device->CreateDepthStencilState(&desc, m_depthStencilState_shadow.ReleaseAndGetAddressOf());
}



/**
 * @brief 影の設定
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void StageScene::SettingShadow(RenderContext context)
{
	// エフェクトの設定・適応
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(context.view);
	m_basicEffect->SetProjection(context.proj);
	m_basicEffect->SetTexture(m_textures->shadow);
	m_basicEffect->Apply(context.deviceContext);

	// 入力レイアウト
	context.deviceContext->IASetInputLayout(m_inputLayout.Get());

	// テクスチャサンプラー
	ID3D11SamplerState* sampler[] = { context.states->LinearClamp() };
	context.deviceContext->PSSetSamplers(0, 1, sampler);

	// アルファブレンド
	context.deviceContext->OMSetBlendState(context.states->AlphaBlend(), nullptr, 0XFFFFFFFF);

	// 深度バッファ
	context.deviceContext->OMSetDepthStencilState(m_depthStencilState_shadow.Get(), 1);
}



/**
 * @brief 影の描画
 *
 * @param position	表示位置
 * @param radius	半径
 *
 * @return なし
 */
void StageScene::DrawShadow(const DirectX::SimpleMath::Vector3 position, const float radius)
{
	// 頂点の設定
	std::array<DirectX::VertexPositionTexture, SHADOW_VERTEX_NUM> vertexes = CreateVertexes(position, radius);
	uint16_t indexes[] = { 2,3,1,2,1,0 };

	// 影の描画
	m_primitiveBatch->DrawIndexed(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		indexes, _countof(indexes),
		vertexes.data(), vertexes.size()
	);
}



/**
 * @brief 影用の頂点の作成
 *
 * @param position	表示位置
 * @param radius	半径
 *
 * @return 頂点
 */
std::array<DirectX::VertexPositionTexture, StageScene::SHADOW_VERTEX_NUM> StageScene::CreateVertexes(
	const DirectX::SimpleMath::Vector3 position, const float radius)
{
	std::array<DirectX::VertexPositionTexture, SHADOW_VERTEX_NUM> vertexes =
	{
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(0.0f,0.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(1.0f,0.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(0.0f,1.0f)),
		DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3::Zero,DirectX::SimpleMath::Vector2(1.0f,1.0f))
	};

	// 位置と半径を反映
	vertexes[0].position = DirectX::SimpleMath::Vector3(-radius, SHADOW_HEIGHT_ADJUST, -radius) + position;
	vertexes[1].position = DirectX::SimpleMath::Vector3(radius, SHADOW_HEIGHT_ADJUST, -radius) + position;
	vertexes[2].position = DirectX::SimpleMath::Vector3(-radius, SHADOW_HEIGHT_ADJUST, radius) + position;
	vertexes[3].position = DirectX::SimpleMath::Vector3(radius, SHADOW_HEIGHT_ADJUST, radius) + position;

	return vertexes;
}
