/**
 * @file   NumberRenderer2D.h
 *
 * @brief  NumberRenderer2Dに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/NumberRenderer/INumberRenderer.h"


// クラスの定義 ===============================================================
/**
 * @brief NumberRenderer2D
 */
class NumberRenderer2D :public INumberRenderer
{
	// クラス定数の宣言 -------------------------------------------------
public:



	// データメンバの宣言 -----------------------------------------------
private:
	DirectX::SimpleMath::Vector2 m_position;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NumberRenderer2D(
		DirectX::SimpleMath::Vector2 spriteSize,
		ID3D11ShaderResourceView* texture,
		int digit);

	// デストラクタ
	~NumberRenderer2D();


	// 操作
public:
	// 初期化処理
	void Initialize(const int& number) override;

	// 描画処理
	void Draw(RenderContext& renderContext) override;

	// 終了処理
	void Finalize() override;

	// 取得/設定
public:
	// 座標を設定
	void SetPosition(DirectX::SimpleMath::Vector2 pos) { m_position = pos; }

	// 内部実装
private:

};
