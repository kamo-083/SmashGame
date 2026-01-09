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
		TextureInfo text_conditions;
		TextureInfo text_keys;
		TextureInfo text_pauseOptions;
		TextureInfo text_pauseTitle;
		TextureInfo text_number;
		TextureInfo text_clearTime;

		// その他
		TextureInfo base_key;
	};

	// 画像サイズ
	struct TexturesSize
	{
		// アイコン
		static constexpr DirectX::SimpleMath::Vector2 ICON_ATK = { 200.0f,200.0f };			// 攻撃UIアイコン
		static constexpr DirectX::SimpleMath::Vector2 ICON_CAM = { 200.0f,135.0f };			// カメラアイコン

		// ウィンドウ
		static constexpr DirectX::SimpleMath::Vector2 WINDOW_RESULT = { 500.0f, 400.0f };	// リザルトウィンドウ
		static constexpr DirectX::SimpleMath::Vector2 WINDOW_PAUSE = { 350.0f, 267.0f };	// ポーズウィンドウ

		// 矢印
		static constexpr DirectX::SimpleMath::Vector2 ARROW_NOMAL = { 200.0f, 100.0f };		// 矢印(通常)
		static constexpr DirectX::SimpleMath::Vector2 ARROW_ROTATE = { 200.0f, 200.0f };	// 矢印(回転)

		// テキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_CONDITION = { 300.0f, 90.0f };	// クリア条件テキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_TITLE = { 350.0f, 70.0f };		// ポーズタイトルテキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_CLEARTIME = { 480.0f,144.0f };	// クリアタイムテキスト
		static constexpr DirectX::SimpleMath::Vector2 TEXT_KEYS = { 120.0f, 120.0f };		// 入力キーテキスト(1文字分)
		static constexpr DirectX::SimpleMath::Vector2 TEXT_OPTION = { 350.0f, 60 };			// ポーズ選択肢テキスト(1行分)
		static constexpr DirectX::SimpleMath::Vector2 TEXT_NUMBER = { 48.0f,72.0f };		// 数字(1文字分)

		// その他
		static constexpr DirectX::SimpleMath::Vector2 KEY_BASE = { 150.0f, 150.0f };		// キーアイコンのベース

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
