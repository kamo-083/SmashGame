/**
 * @file   Button.h
 *
 * @brief  ボタンUIに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/UIElement.h"

// クラスの定義 ===============================================================
/**
 * @brief ボタンUI
 */
class Button :public UIElement
{
// クラス定数の宣言 -------------------------------------------------
private:

// データメンバの宣言 -----------------------------------------------
private:
	// 押された時に実行する処理
	std::function<void()> m_operate;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Button();

	// デストラクタ
	~Button();

// 操作
public:
	// 初期化処理
	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		const Tween2D::TweenData& data,
		const DirectX::SimpleMath::Vector2& size,
		std::function<void()> operate = nullptr);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

	// 終了処理
	void Finalize() override;

	// 押した時の処理
	void Press();

	// トゥイーンのリセット
	void Reset();

// 取得/設定
public:
	void SetTexture(
		ID3D11ShaderResourceView* texture, 
		const DirectX::SimpleMath::Vector2& size = DirectX::SimpleMath::Vector2::Zero);

	// 内部実装
private:

};