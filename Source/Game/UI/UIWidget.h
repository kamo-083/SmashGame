/**
 * @file   UIWidget.h
 *
 * @brief  UIWidgetに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Tween.h"


// クラスの定義 ===============================================================
/**
 * @brief UIWidget
 */
class UIWidget
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// テクスチャ
	ID3D11ShaderResourceView* m_texture;

	// UIのパラメータ
	Tween::UIParams m_params;

	// 画像サイズ
	DirectX::SimpleMath::Vector2 m_texSize;

	std::unique_ptr<Tween> m_tween;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UIWidget();

	// デストラクタ
	~UIWidget();


// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture, const Tween::TweenData data, DirectX::SimpleMath::Vector2 size);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(DirectX::SpriteBatch* spriteBatch);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
