/**
 * @file   InputGuideUI.h
 *
 * @brief  操作ガイドUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"
#include"Source/Game/Data/TextureData.h"


// クラスの定義 ===============================================================
/**
 * @brief 操作ガイドUI
 */
class InputGuideUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct Textures
	{
		TextureInfo base;	// アイコンのベース
		TextureInfo text;	// テキスト
	};

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

	// テキスト画像情報
	std::unique_ptr<TextureInfo> m_textTextureInfo;


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
		Textures textures,
		DirectX::SimpleMath::Vector2 pos,
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
