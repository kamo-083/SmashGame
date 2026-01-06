/**
 * @file   InputHintUI.h
 *
 * @brief  操作方法UIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/UI/Elements/Atlas/ActionAtlas.h"


// クラスの定義 ===============================================================
/**
 * @brief 操作方法UI
 */
class InputHintUI
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// テクスチャ群
	struct Textures
	{
		ID3D11ShaderResourceView* key;		// キー
		ID3D11ShaderResourceView* action;	// 動作
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// テクスチャ群
	std::unique_ptr<Textures> m_textures;

	// 表示位置
	DirectX::SimpleMath::Vector2 m_position;

	// キーテキストの切り取り範囲
	std::vector<RECT> m_keyTextRect;

	// 動作テキストの切り取り範囲
	RECT m_actionTextRect;

	// 大きさ
	float m_scale;

	// １文字の大きさ(縦横共通)
	long m_textSize;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	InputHintUI();

	// デストラクタ
	~InputHintUI();


// 操作
public:
	// 初期化処理
	void Initialize(
		Textures textures,
		DirectX::SimpleMath::Vector2 position,
		float scale,
		long textSize,
		std::vector<DirectX::Keyboard::Keys> keys,
		ActionAtlas::ActionType action);

	// 描画処理
	void Draw(RenderContext context);

	// 終了処理
	void Finalize();

// 取得/設定
public:
	// UI全体の横幅を取得
	float GetWidth();

// 内部実装
private:
	// 幅を算出
	float CalculateWidth(const RECT rect);
};
