/**
 * @file   UITextureCatalog.h
 *
 * @brief  UIで使用する画像のカタログに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Data/TextureData.h"

// クラスの宣言 ===============================================================
class ResourceManager;

// クラスの定義 ===============================================================
/**
 * @brief UI使用画像のカタログ
 */
class UITextureCatalog
{
// クラス定数の宣言 -------------------------------------------------
private:
	// テクスチャ群
	struct UITextures
	{
		// アイコン
		TextureInfo icon_key;			// 操作キーのベース
		TextureInfo icon_attackBasic;	// 通常攻撃
		TextureInfo icon_attackRolling;	// 転がり攻撃
		TextureInfo icon_attackHeavy;	// 重量攻撃
		TextureInfo icon_camera;		// カメラ
		TextureInfo icon_stampOn;		// スタンプ(有)
		TextureInfo icon_stampOff;		// スタンプ(無)

		// ウィンドウ
		TextureInfo window_stageSelect;	// ステージ選択
		TextureInfo window_result;		// リザルト
		TextureInfo window_pause;		// ポーズ

		// 矢印
		TextureInfo arrow_normal;	// 通常
		TextureInfo arrow_rotate;	// 回転

		// 文字
		TextureInfo text_titleLogo;		// タイトルロゴ
		TextureInfo text_titleStart;	// ゲーム開始
		TextureInfo text_titleExit;		// ゲーム終了
		TextureInfo text_keyAction;		// キーの動作
		TextureInfo text_conditions;	// ゴール開放条件
		TextureInfo text_keys;			// キー
		TextureInfo text_pauseOptions;	// ポーズの選択肢
		TextureInfo text_pauseTitle;	// ポーズのタイトル
		TextureInfo text_number;		// 数字
		TextureInfo text_clearTime;		// クリアタイム
	};

	// 画像サイズ
	struct TexturesSize
	{
		// アイコン
		static constexpr DirectX::SimpleMath::Vector2 ICON_ATTACK = { 200.0f,200.0f };		// 攻撃アイコン
		static constexpr DirectX::SimpleMath::Vector2 ICON_CAM = { 200.0f,135.0f };			// カメラアイコン
		static constexpr DirectX::SimpleMath::Vector2 ICON_KEY = { 150.0f, 150.0f };		// 操作キーのベース
		static constexpr DirectX::SimpleMath::Vector2 ICON_STAMP = { 250.0f,250.0f };		// スタンプアイコン

		// ウィンドウ
		static constexpr DirectX::SimpleMath::Vector2 WINDOW_STAGESELECT = { 350.0f,400.0f };	// ステージ選択ウィンドウ
		static constexpr DirectX::SimpleMath::Vector2 WINDOW_RESULT = { 500.0f, 400.0f };		// リザルトウィンドウ
		static constexpr DirectX::SimpleMath::Vector2 WINDOW_PAUSE = { 350.0f, 267.0f };		// ポーズウィンドウ

		// 矢印
		static constexpr DirectX::SimpleMath::Vector2 ARROW_NOMAL = { 200.0f, 100.0f };		// 矢印(通常)
		static constexpr DirectX::SimpleMath::Vector2 ARROW_ROTATE = { 200.0f, 200.0f };	// 矢印(回転)

		// テキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_TITLELOGO = { 640.0f,360.0f };	// タイトルロゴ
		static constexpr DirectX::SimpleMath::Vector2 TEXT_TITLEOPTION = { 400.0f,67.0f };	// タイトル選択肢テキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_CONDITION = { 300.0f, 90.0f };	// クリア条件テキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_TITLE = { 350.0f, 70.0f };		// ポーズタイトルテキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_CLEARTIME = { 480.0f,144.0f };	// クリアタイムテキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_KEYACTION = { 600.0f, 120.0f };	// タイトル・ステージ選択操作テキスト(1行分)
		static constexpr DirectX::SimpleMath::Vector2 TEXT_KEYS = { 120.0f, 120.0f };		// 入力キーテキスト(1文字分)
		static constexpr DirectX::SimpleMath::Vector2 TEXT_PAUSEOPTION = { 350.0f, 60 };	// ポーズ選択肢テキスト(1行分)
		static constexpr DirectX::SimpleMath::Vector2 TEXT_NUMBER = { 48.0f,72.0f };		// 数字(1文字分)
	};

// データメンバの宣言 -----------------------------------------------
private:
	// テクスチャ情報群
	std::unique_ptr<UITextures> m_textures;

	// 読み込み済みか
	bool m_loaded;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UITextureCatalog();

	// デストラクタ
	~UITextureCatalog();

// 操作・取得
public:
	// 画像の読み込み
	void Load(ResourceManager* pRM);

	// テクスチャ情報を取得
	const UITextures& GetTextures() { return *m_textures.get(); }

// 内部実装
private:

};