/**
 * @file   NumberRenderer2D.h
 *
 * @brief  2Dのスプライト数字に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/NumberRenderer/INumberRenderer.h"


// クラスの定義 ===============================================================
/**
 * @brief 2Dのスプライト数字
 */
class NumberRenderer2D :public INumberRenderer
{
	// クラス定数の宣言 -------------------------------------------------
public:



	// データメンバの宣言 -----------------------------------------------
private:
	// 表示位置
	DirectX::SimpleMath::Vector2 m_position;

	// スケール
	float m_scale;

	// 描画時にスプライトバッチのBegin/Endをするか
	bool m_useBeginEnd;


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
	// 表示位置を設定
	void SetPosition(const DirectX::SimpleMath::Vector2 pos) { m_position = pos; }

	// スケールを設定
	void SetScale(const float scale) { m_scale = scale; }

	// スプライトバッチのBegin/Endをするかの設定
	void SetUseBeginEnd(bool use = true) { m_useBeginEnd = use; }

	// 全体の幅を取得
	float GetWidth() { return SPRITE_SIZE.x * NUM_DIGIT * m_scale; }


	// 内部実装
private:

};
