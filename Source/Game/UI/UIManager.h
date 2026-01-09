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
class UITextureCatalog;
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
	// ディマーの暗さ
	static constexpr float DIMMER_DARKNESS = 0.5f;


	// データメンバの宣言 -----------------------------------------------
private:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// UI使用画像のカタログ
	std::shared_ptr<UITextureCatalog> m_textureCatalog;

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
		std::shared_ptr<UITextureCatalog> textureCatalog);

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
