/**
 * @file   UIWidget.h
 *
 * @brief  UIウィジェット(アニメーション付きUIのベース)に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Tween/TweenObject.h"

// クラスの定義 ===============================================================
/**
 * @brief UIウィジェット(アニメーション付きUIのベース)
 */
class UIWidget	: public TweenObject
{
// クラス定数の宣言 -------------------------------------------------
private:

// データメンバの宣言 -----------------------------------------------
private:
	// UIのパラメータ
	Tween2D::TweenParams m_params;
	// トゥイーン
	std::unique_ptr<Tween2D> m_tween;

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
	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		const Tween2D::TweenData& data,
		const DirectX::SimpleMath::Vector2& size,
		bool play = true);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;
	void Draw(
		DirectX::SpriteBatch* spriteBatch,
		ID3D11ShaderResourceView* texture = nullptr,
		const DirectX::SimpleMath::Vector2& pos = DirectX::SimpleMath::Vector2::Zero,
		const RECT* rect = nullptr,
		float rot = FLT_MAX,
		const DirectX::SimpleMath::Color& col = DirectX::SimpleMath::Color(1, 1, 1));

	// 終了処理
	void Finalize();

	// トゥイーンをリセット
	void TweenReset(bool play = true);

// 取得/設定
public:
	// トゥイーンの取得
	Tween2D* GetTween() { return m_tween.get(); }
	
	//現在のパラメータの取得
	Tween2D::TweenParams GetParam() const { return m_params; }
	// 変化後のパラメータを取得
	Tween2D::TweenParams GetDelta () const { return m_tween->GetTweenData().delta; }

	// イージング済みの進行度を取得
	float GetEasingProgress() const { return m_tween->GetEasingProgress(); }
	// 素の経過時間を取得
	float GetLinearProgress() const { return m_tween->GetLinearProgress(); }

	// パラメータを設定
	void SetParam(const Tween2D::TweenParams& start, const Tween2D::TweenParams& delta);
	// イージングの種類を設定
	void SetEaseType(const Easing::EaseType type);

// 内部実装
private:

};