/**
 * @file   StageResultUI.h
 *
 * @brief  リザルトUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/UIElement.h"


// クラスの定義 ===============================================================
/**
 * @brief リザルトUI
 */
class StageResultUI :public UIElement
{
	// クラス定数の宣言 -------------------------------------------------
public:


	// データメンバの宣言 -----------------------------------------------
private:


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageResultUI();

	// デストラクタ
	~StageResultUI();


	// 操作
public:
	// 初期化処理
	void Initialize(ID3D11ShaderResourceView* texture,
		DirectX::SimpleMath::Vector2 texSize,
		DirectX::SimpleMath::Vector2 windowSize);

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
