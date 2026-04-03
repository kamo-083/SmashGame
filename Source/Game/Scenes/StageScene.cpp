/**
 * @file   StageScene.cpp
 *
 * @brief  ステージシーンに関するソースファイル
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StageScene.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Transition/BlockTransition.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Physics/CollisionManager.h"
#include "Source/Game/Effect/EffectManager.h"
#include "Source/Game/Data/PlayerInfoLoader.h"
#include "Source/Game/GameObjects/Camera.h"
#include "Source/Game/GameObjects/Player/Player.h"
#include "Source/Game/GameObjects/Enemy/EnemyManager.h"
#include "Source/Game/GameObjects/Stage/StageManager.h"
#include "Source/Game/GameObjects/Stage/Objects/Ground.h"
#include "Source/Game/GameObjects/Sky.h"
#include "Source/Game/UI/UIManager.h"
#include "Source/Game/UI/Controls/AttackUI.h"
#include "Source/Game/UI/Displays/StageResultUI.h"
#include "Source/Game/UI/Displays/PauseUI.h"

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
	SceneManager* pSM, UserResources* pUR, const int& stageNum,
	const std::string& path, ClearConditionsUI::ConditionsType clearCondition)
	:
	Scene{ pSM,pUR },
	CLEAR_CONDITIONS{ clearCondition },
	STAGE_NUM{ stageNum },
	m_keyMode{ false },
	m_timer{ 0.0f },
	m_stageFilePath{ path },
	m_overlayMode{ Overlay::NONE },
	m_keyConfig{}
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

	// 経過時間の初期化
	m_timer = 0.0f;

	// メッセンジャーの作成
	m_messenger = std::make_unique<Messenger>();

	// 当たり判定マネージャーの作成
	m_collisionManager = std::make_unique<CollisionManager>();

	// レイヤーフィルターの登録
	SetupCollitionLayer();

	// カメラの作成
	m_camera = std::make_unique<Camera>();
	// メッセンジャーに登録
	m_messenger->AddObject(m_camera->GetListenerID(), m_camera.get());

	// エフェクトマネージャーの作成
	SetupEffects(pDR);

	// ウィンドウサイズの取得
	DirectX::SimpleMath::Vector2 windowSize = DirectX::SimpleMath::Vector2(
		static_cast<float>(pDR->GetOutputSize().right), static_cast<float>(pDR->GetOutputSize().bottom));

	// 操作キー設定の読み込み
	InputKeyLoader::InputKeyInfo keyConfig;
	InputKeyLoader::LoadData("Resources/Json/inputKeyInfo.json", keyConfig);
	m_keyConfig = keyConfig;

	// UIマネージャーの作成
	m_UIManager = std::make_unique<UIManager>(windowSize, m_sceneManager->GetUITextureCatalog());
	m_UIManager->SetupStageUI(pDR, pAM, m_userResources->GetKeyboardTracker(), CLEAR_CONDITIONS, keyConfig);

	// 深度ステンシルステートの作成
	CreateDepthStencilState(pDR->GetD3DDevice());

	// プレイヤーの作成
	SetupPlayer(pRM);

	// 敵マネージャーの作成
	SetupEnemy();

	// ステージマネージャーの作成
	SetupStage();

	// カメラの初期化
	m_camera->Initialize(&m_player->GetPosition());

	// スカイドームの作成
	SetupSkydome();

	// キー操作のモードの初期化
	m_keyMode = true;

	// 表示状態の初期化
	m_overlayMode = Overlay::GAMEPLAY;

	// テクスチャの設定
	SetupTextures(pRM);

	// 音声の設定
	SetupSounds(pAM);

	// シーン遷移演出を開く
	BlockTransition* transition = m_sceneManager->GetTransition();
	if (transition->IsClose())	transition->Open();
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
	// モードに応じて更新
	switch (m_overlayMode)
	{
	case StageScene::Overlay::GAMEPLAY:
		UpdateGameplay(elapsedTime);
		break;
	case StageScene::Overlay::PAUSE:
		UpdatePause(elapsedTime);
		break;
	case StageScene::Overlay::RESULT:
		UpdateResult(elapsedTime);
		break;
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
void StageScene::Render(RenderContext& context, DebugFont* debugFont)
{
	// デバッグ情報の追加
	debugFont->AddString(0, 30, DirectX::Colors::White, L"StageScene");
	debugFont->AddString(150, 30, DirectX::Colors::Red, L"mode=%d", m_overlayMode);
	debugFont->AddString(250, 30, DirectX::Colors::Red, L"pause=%d", m_UIManager->GetPauseUI()->GetNowOption());

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
	DrawObjectsShadow(context);

	// エフェクトの描画
	m_effectManager->Draw(context.proj);

	// カメラの描画(デバッグフォント)
	m_camera->Draw(debugFont);

	// UIの描画
	m_UIManager->Draw(context);
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

	if (m_sky) m_sky->Finalize();
	m_sky.reset();

	if (m_UIManager) m_UIManager->Finalize();
	m_UIManager.reset();

	if (m_enemyManager) m_enemyManager->Finalize();
	m_enemyManager.reset();

	if (m_stageManager) m_stageManager->Finalize();
	m_stageManager.reset();

	if (m_effectManager) m_effectManager->Finalize();
	m_effectManager.reset();

	m_camera.reset();
	m_collisionManager.reset();

	m_textures.reset();
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

	// 床用
	// 深度テスト
	desc.DepthEnable = TRUE;									// 深度テストを有効化
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// 深度バッファに書き込みをする
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;				// 通常の深度比較
	// ステンシルテスト
	desc.StencilEnable = TRUE;									// ステンシルテストを有効化 
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 初期値(0xff)
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;	// 初期値(0xff)
	// 表面
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		// 常に1
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;	// 描画成功時に StencilRef を書き込む
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;		// そのまま
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// そのまま
	// 裏面
	desc.BackFace = desc.FrontFace;								// 裏面も表面と同じ
	// 深度ステンシルステートの作成
	device->CreateDepthStencilState(&desc, m_depthStencilState_stage.ReleaseAndGetAddressOf());

	// 影用
	// 深度テスト
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// 深度バッファに書き込みしない
	// ステンシルテスト
	desc.StencilWriteMask = 0x00;	// ステンシル書き込み禁止
	// 表面
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;		// (StencilValue == Ref) のとき描画
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;		// ステンシルを書き換えない
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
void StageScene::SetupShadow(const RenderContext& context)
{
	// エフェクトの設定・適応
	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_basicEffect->SetView(context.view);
	m_basicEffect->SetProjection(context.proj);
	m_basicEffect->SetTexture(m_textures->shadow.Get());
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

	// カリングなし
	context.deviceContext->RSSetState(context.states->CullNone());
}

/**
 * @brief 影の描画
 *
 * @param position	表示位置
 * @param radius	半径
 *
 * @return なし
 */
void StageScene::DrawShadow(const DirectX::SimpleMath::Vector3& position, const float radius)
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
	const DirectX::SimpleMath::Vector3& position, const float radius)
{
	// 4頂点を作成
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

/**
 * @brief オブジェクトの影を描画
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void StageScene::DrawObjectsShadow(const RenderContext& context)
{
	// 影の設定
	SetupShadow(context);
	float groundHeight = m_stageManager->GetGround(0)->GetHeight().y;

	m_primitiveBatch->Begin();

	// プレイヤー
		// Y座標が地面以上なら描画
	if (m_player->GetPosition().y >= groundHeight)
	{
		DrawShadow(
			DirectX::SimpleMath::Vector3(m_player->GetPosition().x, groundHeight, m_player->GetPosition().z),
			m_player->GetRadius());
	}

	// 敵
	for (int i = 1; i <= m_enemyManager->GetEnemyNum(); i++)
	{
		// 敵のポインタを取得
		IEnemy* enemy = m_enemyManager->GetEnemyByID(i);

		// 対象が存在しなかったら飛ばす
		if (!enemy) continue;

		// Y座標が地面未満なら飛ばす
		if (enemy->GetPosition().y < groundHeight) continue;

		// 座標を取得
		DirectX::SimpleMath::Vector3 enemyPos = enemy->GetPosition();
		enemyPos.y = groundHeight;
		// 半径を取得
		float enemyRadius = enemy->GetRadius();
		// 影を描画
		DrawShadow(enemyPos, enemyRadius);
	}

	m_primitiveBatch->End();
}

/**
 * @brief ゲームプレイ中の更新
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void StageScene::UpdateGameplay(float elapsedTime)
{
	// キー操作のモード切り替え
	if (m_userResources->GetKeyboardTracker()->IsKeyPressed(m_keyConfig.mode_switch))
	{
		ChangeKeyMode();
	}

	// キー入力による操作
	KeyOperation();

	// 経過時間の加算
	m_timer += elapsedTime;

	// プレイヤーの更新
	m_player->Update(elapsedTime);	

	// 敵の更新
	m_enemyManager->Update(elapsedTime, m_player.get());

	// カメラの更新
	m_camera->Update(elapsedTime);

	// ステージの更新
	m_stageManager->Update(elapsedTime, m_camera->GetEye(), m_camera->GetUp());

	// エフェクトの更新
	m_effectManager->Update(elapsedTime);

	// UIの更新
	m_UIManager->Update(elapsedTime);

	// 当たり判定の更新
	m_collisionManager->Update();

	// 落下時のリスポーン
	if (m_player->GetPosition().y <= m_player->GetKillHeight()) m_player->Respawn();

	// リザルトの表示　
	if (m_stageManager->IsGoal())
	{
		// リザルトUIの有効化
		m_UIManager->GetResultUI()->SetClearTime(m_timer);
		m_UIManager->GetResultUI()->OpenResult();

		m_overlayMode = Overlay::RESULT;
	}

	// ポーズに切り替え
	if (m_userResources->GetKeyboardTracker()->pressed.Q)
	{
		m_overlayMode = Overlay::PAUSE;

		m_UIManager->GetPauseUI()->OpenPause();
	}
}

/**
 * @brief ポーズ中の更新
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void StageScene::UpdatePause(float elapsedTime)
{
	DirectX::Keyboard::KeyboardStateTracker* kbTracker = m_userResources->GetKeyboardTracker();

	m_UIManager->GetPauseUI()->Update(elapsedTime);

	// 選択項目を切り替え
	if (kbTracker->pressed.Up)
	{
		// 選択項目を上へ
		m_UIManager->GetPauseUI()->SelectUp();
	}
	else if (kbTracker->pressed.Down)
	{
		// 選択項目を下へ
		m_UIManager->GetPauseUI()->SelectDown();
	}

	BlockTransition* transition = m_sceneManager->GetTransition();

	// スペースキーで項目を選択
	if (kbTracker->pressed.Space)
	{
		switch (m_UIManager->GetPauseUI()->GetNowOption())
		{
		case PauseUI::PAUSE_OPTIONS::RETURN_STAGE:	// ゲームプレイへ
			m_overlayMode = Overlay::GAMEPLAY;
			m_UIManager->GetPauseUI()->ClosePause();
			break;
		case PauseUI::PAUSE_OPTIONS::STAGE_SELECT:	// ステージ選択へ
			// シーン遷移演出
			if (transition->IsOpen())
			{
				// シーンを閉じる
				transition->Close();
			}
			break;
		case PauseUI::PAUSE_OPTIONS::RESET_STAGE:	// ステージをリセット
			Initialize();
			break;
		}
	}

	// ゲームプレイに戻る
	if (kbTracker->pressed.Q)
	{
		m_overlayMode = Overlay::GAMEPLAY;

		m_UIManager->GetPauseUI()->ClosePause();
	}

	// シーン遷移演出が終わっていたら
	if (transition->IsClose() && transition->IsEnd())
	{
		// BGMの停止
		m_userResources->GetAudioManager()->Stop("stageBGM");
		// シーン移動
		ChangeScene("StageSelectScene");
	}
}

/**
 * @brief リザルト中の更新
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void StageScene::UpdateResult(float elapsedTime)
{
	m_UIManager->GetResultUI()->Update(elapsedTime);

	// シーン遷移演出
	BlockTransition* transition = m_sceneManager->GetTransition();
	if (m_userResources->GetKeyboardTracker()->pressed.Space && transition->IsOpen())
	{
		// シーンを閉じる
		transition->Close();
	}
	if (transition->IsClose() && transition->IsEnd())
	{
		// クリア情報を登録
		m_sceneManager->SetSharedData("cleared" + std::to_string(STAGE_NUM), "true");
		// BGMの停止
		m_userResources->GetAudioManager()->Stop("stageBGM");
		// シーン移動
		ChangeScene("StageSelectScene");
	}
}

/**
 * @brief キー入力による操作
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::KeyOperation()
{	
	using MessageID = Message::MessageID;

	// キーボードの状態を取得
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker = m_userResources->GetKeyboardTracker();	// キーボードトラッカーのポインタ
	DirectX::Keyboard::State keyLastState = pKbTracker->GetLastState();								// 最後の状態

	// プレイヤー
	// 待機フラグ
	bool player_idle = true;
	// 前移動
	if (keyLastState.IsKeyDown(m_keyConfig.move_forward))
	{
		player_idle = false;
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_MOVE_FORWARD);
	}
	// 後ろ移動
	else if (keyLastState.IsKeyDown(m_keyConfig.move_backward))
	{
		player_idle = false;
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_MOVE_BACKWARD);
	}
	// 左移動
	if (keyLastState.IsKeyDown(m_keyConfig.move_left))
	{
		player_idle = false;
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_MOVE_LEFT);
	}
	// 右移動
	else if (keyLastState.IsKeyDown(m_keyConfig.move_right))
	{
		player_idle = false;
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_MOVE_RIGHT);
	}
	// 移動操作が無ければ待機
	if (player_idle)
	{
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_IDLE);
	}
	// 攻撃
	if (pKbTracker->IsKeyPressed(m_keyConfig.attack))
	{
		m_messenger->Notify(m_player->GetListenerID(), MessageID::PLAYER_ATTACK);
	}

	// 攻撃変更
	if (m_keyMode)
	{
		// 左側の攻撃にする
		if (pKbTracker->IsKeyPressed(m_keyConfig.rotate_left))
		{
			m_messenger->Notify(m_player->GetListenerID(), MessageID::ATTACK_CHANGE_LEFT);
		}
		// 右側の攻撃にする
		else if (pKbTracker->IsKeyPressed(m_keyConfig.rotate_right))
		{
			m_messenger->Notify(m_player->GetListenerID(), MessageID::ATTACK_CHANGE_RIGHT);
		}
	}
	// カメラ
	else
	{
		// 左回転
		if (pKbTracker->IsKeyPressed(m_keyConfig.rotate_left))
		{
			m_messenger->Notify(m_camera->GetListenerID(), Message::MessageID::CAMERA_ROTATE_LEFT);
		}
		// 右回転
		else if (pKbTracker->IsKeyPressed(m_keyConfig.rotate_right))
		{
			m_messenger->Notify(m_camera->GetListenerID(), Message::MessageID::CAMERA_ROTATE_RIGHT);
		}
	}
}

/**
 * @brief キー操作モードの切り替え
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::ChangeKeyMode()
{
	// モードを反転
	m_keyMode = !m_keyMode;

	// UI切り替え
	m_UIManager->GetCameraUI()->Active(!m_UIManager->GetCameraUI()->IsActive());
	m_UIManager->GetAttackUI()->SwitchUIMode();
}

/**
 * @brief 当たり判定のレイヤーフィルターの設定
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::SetupCollitionLayer()
{
	auto& M = m_collisionManager->GetLayerMatrix();
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyBody] = true;		// プレイヤーと敵
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyBody] = true;		// 敵と敵
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::EnemyAttack] = true;	// プレイヤーと敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::PlayerAttack] = false;	// プレイヤーとプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::PlayerAttack] = true;	// 敵とプレイヤーの攻撃
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::EnemyAttack] = false;	// 敵と敵の攻撃
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Stage] = true;			// プレイヤーとステージ
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Stage] = true;			// 敵とステージ
	M.matrix[(int)CollisionManager::Layer::PlayerBody][(int)CollisionManager::Layer::Trigger] = true;		// プレイヤーとトリガー
	M.matrix[(int)CollisionManager::Layer::EnemyBody][(int)CollisionManager::Layer::Trigger] = true;		// 敵とトリガー
}

/**
 * @brief エフェクトマネージャーの設定
 *
 * @param pDR	デバイスリソースのポインタ
 *
 * @return なし
 */
void StageScene::SetupEffects(DX::DeviceResources* pDR)
{
	m_effectManager = std::make_unique<EffectManager>(pDR, m_userResources->GetStates());
	m_effectManager->LoadShaders(m_userResources->GetShaderManager());
	m_effectManager->SetCamera(m_camera.get());
}

/**
 * @brief プレイヤーの設定
 *
 * @param pRM		リソースマネージャーのポインタ
 *
 * @return なし
 */
void StageScene::SetupPlayer(ResourceManager* pRM)
{
	PlayerInfoLoader loader;
	PlayerInfoLoader::PlayerInfo info;
	loader.LoadData("Resources/Json/playerInfo.json", info);
	Player::PlayerParams param =
	{
			pRM,
			m_collisionManager.get(),
			m_camera.get(),
			m_UIManager->GetAttackUI(),
			info
	};
	m_player = std::make_unique<Player>(m_userResources, m_effectManager.get(), info);
	m_player->Initialize(param, m_keyConfig);
	m_messenger->AddObject(m_player->GetListenerID(), m_player.get());
}

/**
 * @brief 敵の設定
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::SetupEnemy()
{
	m_enemyManager = std::make_unique<EnemyManager>(
		m_userResources, m_collisionManager.get(), m_effectManager.get());
	m_enemyManager->Initialize();
}

/**
 * @brief ステージの設定
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::SetupStage()
{
	m_stageManager = std::make_unique<StageManager>(m_depthStencilState_stage.Get());
	m_stageManager->CreateStage(
		m_userResources, m_collisionManager.get(), m_enemyManager.get(), m_effectManager.get(),
		m_stageFilePath);
}

/**
 * @brief スカイドームの設定
 *
 * @param なし
 *
 * @return なし
 */
void StageScene::SetupSkydome()
{
	m_sky = std::make_unique<Sky>();
	m_sky->Initialize(m_userResources);
	m_sky->SetPosition(&m_player->GetPosition());
}

/**
 * @brief テクスチャの設定
 *
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void StageScene::SetupTextures(ResourceManager* pRM)
{
	// テクスチャの読み込み
	m_textures = std::make_unique<Textures>();
	m_textures->shadow = pRM->RequestDDS("shadow", "Others/shadow.dds");
}

/**
 * @brief 音声の設定
 *
 * @param pAM	オーディオマネージャーのポインタ
 *
 * @return なし
 */
void StageScene::SetupSounds(AudioManager* pAM)
{
	// BGM・SEの読み込み
	pAM->LoadMP3("stageBGM", "BGM/iwashiro_orange_hill.mp3");
	pAM->LoadMP3("startSE", "SE/fue.mp3");
	pAM->LoadMP3("canGoalSE", "SE/bell.mp3");
	pAM->LoadMP3("clearSE", "SE/one08.mp3");
	pAM->LoadMP3("attackSE", "SE/hit01.mp3");
	pAM->LoadMP3("cursorSE", "SE/button68.mp3");
	pAM->LoadMP3("keySpawnSE", "SE/button15.mp3");
	pAM->LoadMP3("gimmicSolveSE", "SE/button43.mp3");
	pAM->LoadMP3("bounceSE", "SE/bounce03.mp3");

	// BGM・SEの音量変更
	pAM->SetVolume("stageBGM", BGM_VOLUME);

	// BGMの再生
	pAM->Play("stageBGM", true);
	// スタートSEの再生
	pAM->Play("startSE", false);
}