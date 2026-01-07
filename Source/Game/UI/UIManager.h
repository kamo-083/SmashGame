/**
 * @file   UIManager.h
 *
 * @brief  UIマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/Data/TextureData.h"
#include "Source/Game/Data/InputKeyLoader.h"
#include "Source/Game/UI/Controls/OperationUI.h"
#include "Source/Game/UI/Displays/ClearConditionsUI.h"


// クラスの宣言 ===============================================================
class ResourceManager;
class UIElement;
class StageResultUI;
class AttackUI;
class PauseUI;
class InputHintUI;
class UIDimmer;


// クラスの定義 ===============================================================
/**
 * @brief UIマネージャー
 */
class UIManager
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// テクスチャ群
	struct UITextures
	{
		// アイコン
		TextureInfo icon_attackBasic;
		TextureInfo icon_attackRolling;
		TextureInfo icon_attackHeavy;
		TextureInfo icon_camera;

		// ウィンドウ
		TextureInfo window_result;
		TextureInfo window_pause;

		// 矢印
		TextureInfo arrow_normal;
		TextureInfo arrow_rotate;

		// 文字
		TextureInfo text_operation;
		TextureInfo text_conditions;
		TextureInfo text_keys;
		TextureInfo text_pauseOptions;
		TextureInfo text_pauseTitle;
		TextureInfo text_number;
		TextureInfo text_clearTime;

		// その他
		TextureInfo base_key;
	};

	// ディマーの暗さ
	static constexpr float DIMMER_DARKNESS = 0.5f;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 TEX_SIZE_ICON_ATK = { 200.0f,200.0f };			// 攻撃UIアイコン
	DirectX::SimpleMath::Vector2 TEX_SIZE_ICON_CAM = { 200.0f,135.0f };			// カメラアイコン
	DirectX::SimpleMath::Vector2 TEX_SIZE_WINDOW_RESULT = { 350.0f, 400.0f };	// リザルトウィンドウ
	DirectX::SimpleMath::Vector2 TEX_SIZE_WINDOW_PAUSE = { 350.0f, 267.0f };	// ポーズウィンドウ
	DirectX::SimpleMath::Vector2 TEX_SIZE_ARROW_NOMAL = { 200.0f, 100.0f };		// 矢印(通常)
	DirectX::SimpleMath::Vector2 TEX_SIZE_ARROW_ROTATE = { 200.0f, 200.0f };	// 矢印(回転)
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_OPERATION = { 50.0f, 100.0f };	// 操作キーテキスト
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_CONDITION = { 300.0f, 90.0f };	// クリア条件テキスト
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_KEYS = { 120.0f, 120.0f };		// 入力キーテキスト(1文字分)
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_OPTION = { 350.0f, 200.0f };		// ポーズ選択肢テキスト
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_TITLE = { 350.0f, 70.0f };		// ポーズタイトルテキスト
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_NUMBER = { 48.0f,72.0f };		// 数字(1文字分)
	DirectX::SimpleMath::Vector2 TEX_SIZE_TEXT_CLEARTIME = { 480.0f,144.0f };	// クリアタイムテキスト
	DirectX::SimpleMath::Vector2 TEX_SIZE_KEY_BASE = { 150.0f, 150.0f };		// キーアイコンのベース


	// データメンバの宣言 -----------------------------------------------
private:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// リソースマネージャーのポインタ
	ResourceManager* m_pRM;

	// テクスチャ群
	std::unique_ptr<UITextures> m_textures;

	// ディマー
	std::unique_ptr<UIDimmer> m_dimmer;

	// 基本UIの配列
	std::vector<std::unique_ptr<UIElement>> m_elements;

	// リザルトUI
	std::unique_ptr<StageResultUI> m_resultUI;

	// 攻撃方法UI
	std::unique_ptr<AttackUI> m_attackUI;

	// カメラ回転UI
	std::unique_ptr<OperationUI> m_cameraUI;

	// ポーズUI
	std::unique_ptr<PauseUI> m_pauseUI;

	// 操作方法UI
	std::vector<std::unique_ptr<InputHintUI>> m_inputHintUI;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UIManager(
		DirectX::SimpleMath::Vector2 windowSize,
		ResourceManager* pRM);

	// デストラクタ
	~UIManager();


// 操作
public:
	// UIの初期化(ステージ用)
	void SetupStageUI(
		DX::DeviceResources* pDR,
		DirectX::Keyboard::KeyboardStateTracker* pKbTracker,
		ClearConditionsUI::ConditionsType conditionsType,
		const InputKeyLoader::InputKeyInfo& keyConfig
		);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);

	// 終了処理
	void Finalize();


// 取得/設定
public:
	// ウィンドウサイズの設定
	void SetWindowSize(DirectX::SimpleMath::Vector2 windowSize) { m_windowSize = windowSize; }

	// リザルトUIの取得
	StageResultUI* GetResultUI() { return m_resultUI.get(); }

	// 攻撃方法UIの取得
	AttackUI* GetAttackUI() { return m_attackUI.get(); }

	// カメラ回転UIの取得
	OperationUI* GetCameraUI() { return m_cameraUI.get(); }

	// ポーズ画面UIの取得
	PauseUI* GetPauseUI() { return m_pauseUI.get(); }


// 内部実装
private:
	// テクスチャの読み込み
	void LoadTextures();

	// ディマーの描画
	void DrawDimmer(UIElement* ui, RenderContext context);

	// リザルトUIの作成
	void CreateResultUI();

	// クリア条件UIの作成
	void CreateClearConditionUI(ClearConditionsUI::ConditionsType type);

	// 操作ガイドUIの作成
	void CreateKeyGuideUI(
		DirectX::SimpleMath::Vector2 position,
		std::vector<DirectX::Keyboard::Keys> keys,
		DirectX::Keyboard::KeyboardStateTracker* pKbTracker
	);

	// 攻撃方法UIの作成
	void CreateAttackUI(OperationUI::OperationUIDesc opUIDesc);

	// カメラ回転UIの作成
	void CreateCameraUI(
		OperationUI::OperationUIDesc opUIDesc,
		DirectX::SimpleMath::Vector2 position
	);

	// ポーズ画面UIの作成
	void CreatePauseUI();

	// 操作方法UIの作成
	void CreateInputHintUI();
};
