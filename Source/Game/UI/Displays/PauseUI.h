/**
 * @file   PauseUI.h
 *
 * @brief  ポーズUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief ポーズUI
 */
class PauseUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 選択項目
	enum class PAUSE_OPTIONS
	{
		NONE = -1,
		RETURN_STAGE,	// ステージに戻る
		STAGE_SELECT,	// ステージ選択へ
		RESET_STAGE,	// ステージを最初からにする

		OPTIONS_NUM		// 項目数
	};

	// テクスチャ群
	struct Textures
	{
		ID3D11ShaderResourceView* window;		// 背景
		ID3D11ShaderResourceView* titleText;	// タイトルのテキスト
		ID3D11ShaderResourceView* optionsText;	// 選択肢のテキスト
	};

	// 引数用構造体
	struct PauseUIDesc
	{
		DirectX::SimpleMath::Vector2 windowTexSize;	// ウィンドウ画像サイズ
		DirectX::SimpleMath::Vector2 titleTexSize;	// タイトル画像サイズ
	};

	// 選択肢テキストの色
	static constexpr DirectX::SimpleMath::Color SELECTED_COLOR = { 1,1,0,1 };			// 選択時
	static constexpr DirectX::SimpleMath::Color UNSELECTED_COLOR = { 0.9,0.9,0.9,1 };	// 非選択時

	static constexpr LONG OPTIONS_HEIGHT = 60.0f;	// 選択肢テキスト画像1行分の高さ
	static constexpr LONG OPTIONS_WIDTH = 350.0f;	// 選択肢テキスト画像の横幅

	static constexpr float TEXT_TITLE_MOVE = -5.0f;	// タイトル画像が動く幅

	static constexpr float TITLE_POS_ADJUST = 170.0f;	// タイトル画像位置の調整

	// アニメーション時間
	static constexpr float WINDOW_ANIM_TIME = 0.15f;	// ウィンドウ
	static constexpr float TITLE_ANIM_TIME = 1.0f;		// タイトル

	// データメンバの宣言 -----------------------------------------------
private:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// タイトル用ウィジェット
	std::unique_ptr<UIWidget> m_titleWidget;

	// 選択している項目
	PAUSE_OPTIONS m_options;

	// 開かれているか
	bool m_isOpen;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseUI();

	// デストラクタ
	~PauseUI();


	// 操作
public:
	// 初期化処理
	void Initialize(
		const DirectX::SimpleMath::Vector2& windowSize,
		const Textures& textures,
		const PauseUIDesc& desc);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;

	// ポーズを開く
	void OpenPause();

	// ポーズを閉じる
	void ClosePause();

	// 選択項目を上に動かす
	void SelectUp();

	// 選択項目を下に動かす
	void SelectDown();


	// 取得/設定
public:
	// 選択中の項目を返す
	const PauseUI::PAUSE_OPTIONS& GetNowOption() { return m_options; }

	// 開いているかを返す
	bool IsOpen() { return m_isOpen; }


	// 内部実装
private:
	// ウィジェットの初期設定
	void SetupWidget(const PauseUIDesc& desc);
};
