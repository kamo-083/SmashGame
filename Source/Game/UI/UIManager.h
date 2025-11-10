/**
 * @file   UIManager.h
 *
 * @brief  UIマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/RenderContext.h"
#include "Source/Game/UI/Controls/OperationUI.h"
#include "Source/Game/UI/Displays/ClearConditionsUI.h"


// クラスの宣言 ===============================================================
class ResourceManager;
class UIElement;
class AttackUI;


// クラスの定義 ===============================================================
/**
 * @brief UIマネージャー
 */
class UIManager
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// テクスチャ情報
	struct TextureDesc
	{
		ID3D11ShaderResourceView* texture;	// ポインタ
		DirectX::SimpleMath::Vector2 size;	// 画像サイズ
	};

	// テクスチャ群
	struct UITextures
	{
		// アイコン
		TextureDesc icon_attackBasic;
		TextureDesc icon_attackRolling;
		TextureDesc icon_attackHeavy;
		TextureDesc icon_camera;

		// ウィンドウ
		TextureDesc window_result;

		// 矢印
		TextureDesc arrow_normal;
		TextureDesc arrow_rotate;

		// 文字
		TextureDesc text_operation;
		TextureDesc text_conditions;
	};

	// データメンバの宣言 -----------------------------------------------
private:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// リソースマネージャーのポインタ
	ResourceManager* m_pRM;

	// テクスチャ群
	std::unique_ptr<UITextures> m_textures;

	// 基本UIの配列
	std::vector<std::unique_ptr<UIElement>> m_elements;

	// 攻撃方法用UI
	std::unique_ptr<AttackUI> m_attackUI;

	// カメラ回転用UI
	std::unique_ptr<OperationUI> m_cameraUI;


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
		DirectX::Keyboard::KeyboardStateTracker* pKbTracker,
		ClearConditionsUI::ConditionsType conditionsType
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


// 内部実装
private:
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
};
