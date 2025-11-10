/**
 * @file   UIElement.h
 *
 * @brief  基本的なUIの基底クラスに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIWidget.h"


// クラスの定義 ===============================================================
/**
 * @brief 基本的なUIの基底クラス
 */
class UIElement
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
protected:
	// ウィジェット
	std::unique_ptr<UIWidget> m_widget;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	UIElement()
	{}

	// デストラクタ
	virtual ~UIElement()
	{}


// 操作
public:
	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Draw(RenderContext context) = 0;

	// 終了処理
	virtual void Finalize() = 0;

// 取得/設定
public:
	// 現在のパラメータの取得
	Tween2D::UIParams GetParam() const { return m_widget->GetParam(); }

// 内部実装
private:

};
