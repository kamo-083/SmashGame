/**
 * @file   InputGuideUI.h
 *
 * @brief  ボタン操作UIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief 操作方法UI
 */
class InputGuideUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:


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
		ID3D11ShaderResourceView* texture,
		DirectX::SimpleMath::Vector2 pos,
		DirectX::SimpleMath::Vector2 size);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(RenderContext context) override;

	// 終了処理
	void Finalize() override;

	// 押した時の処理
	void Press();

	// トゥイーンのリセット
	void Reset();


	// 取得/設定
public:

	// 内部実装
private:

};
