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
		RETURN_STAGE,
		STAGE_SELECT,
		RESET_STAGE,

		OPTIONS_NUM
	};

	struct Textures
	{
		ID3D11ShaderResourceView* window;
		ID3D11ShaderResourceView* text;
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// 選択項目
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
		Textures& textures);

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

	// 内部実装
private:

};
