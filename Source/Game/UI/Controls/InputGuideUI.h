/**
 * @file   InputGuideUI.h
 *
 * @brief  操作ガイドUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief 操作ガイドUI
 */
class InputGuideUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
private:


	// データメンバの宣言 -----------------------------------------------
private:
	// 対応キー
	std::vector<DirectX::Keyboard::Keys> m_inputKeys;

	// キーボードトラッカーのポインタ
	DirectX::Keyboard::KeyboardStateTracker* m_pKbTracker;

	// 押下状態
	bool m_pressed;

	// 対応キー全体の以前の押下状態
	std::vector<bool> m_keyLastStates;

	// 文字テクスチャのポインタ
	ID3D11ShaderResourceView* m_textTexture;

	// テキスト画像の1文字分の幅
	int m_textWidth;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputGuideUI();

	// デストラクタ
	~InputGuideUI();


	// 操作
public:
	// 初期化処理
	void Initialize(
		ID3D11ShaderResourceView* textTexture,
		ID3D11ShaderResourceView* baseTexture,
		DirectX::SimpleMath::Vector2 pos,
		DirectX::SimpleMath::Vector2 size,
		int textWidth,
		std::vector<DirectX::Keyboard::Keys> keys,
		DirectX::Keyboard::KeyboardStateTracker* pKbTracker
	);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;


	// 取得/設定
public:

	// 内部実装
private:

};
