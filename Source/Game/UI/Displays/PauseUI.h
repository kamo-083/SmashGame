/**
 * @file   PauseUI.h
 *
 * @brief  ポーズUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"
#include"Source/Game/Data/TextureData.h"
#include"Source/Game/Object/AudioListener.h"

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
		TextureInfo window;			// 背景
		TextureInfo titleText;		// タイトルのテキスト
		TextureInfo optionsText;	// 選択肢のテキスト
	};

	// 選択肢テキストの色
	static constexpr DirectX::SimpleMath::Color SELECTED_COLOR = { 1,1,0,1 };			// 選択時
	static constexpr DirectX::SimpleMath::Color UNSELECTED_COLOR = { 0.9,0.9,0.9,1 };	// 非選択時

	static constexpr float TEXT_TITLE_MOVE = -5.0f;	// タイトル画像が動く量

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

	// 音声再生機能
	AudioListener m_audio;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	PauseUI(AudioManager* pAM);

	// デストラクタ
	~PauseUI();

	// 操作
public:
	// 初期化処理
	void Initialize(
		const DirectX::SimpleMath::Vector2& windowSize,
		const Textures& textures);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

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
	PauseUI::PAUSE_OPTIONS& GetNowOption() { return m_options; }

	// 開いているかを返す
	bool IsOpen() const { return m_isOpen; }


	// 内部実装
private:
	// ウィジェットの初期設定
	void SetupWidget();
};
