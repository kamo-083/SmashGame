/**
 * @file   BillboardSprite.h
 *
 * @brief  BillboardSpriteに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Tween/TweenObject.h"
#include "Source/Game/Common/DeviceResources.h"

// クラスの定義 ===============================================================
/**
 * @brief BillboardSprite
 */
class BillboardSprite	: public TweenObject
{
// クラス定数の宣言 -------------------------------------------------
private:
	// 頂点の配列
	const static DirectX::VertexPositionTexture VERTECES[4];

// データメンバの宣言 -----------------------------------------------
private:
	// トゥイーンのパラメータ
	Tween3D::TweenParams m_params;
	// トゥイーン
	std::unique_ptr<Tween3D> m_tween;

	// ビルボードかどうか
	bool m_isBillboard;
	// ビルボード行列
	DirectX::SimpleMath::Matrix m_billboard;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	// エフェクト
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	BillboardSprite(DX::DeviceResources* pDR);

	// デストラクタ
	~BillboardSprite();


// 操作
public:
	// 初期化処理
	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		const Tween3D::TweenData& data,
		const DirectX::SimpleMath::Vector2& size);

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Draw(const RenderContext& context) override;

	// 終了処理
	void Finalize();

	// ビルボードの作成
	void CreateBillboard(
		const DirectX::SimpleMath::Vector3& eye,
		const DirectX::SimpleMath::Vector3& up
	);

	// トゥイーンをリセット
	void TweenReset(bool play = true);

// 取得/設定
public:
	// トゥイーンの取得
	Tween3D* GetTween() { return m_tween.get(); }

	//現在のパラメータの取得
	Tween3D::TweenParams GetParam() const { return m_params; }
	// 変化後のパラメータを取得
	Tween3D::TweenParams GetDelta() const { return m_tween->GetTweenData().delta; }

	// イージング済みの進行度を取得
	float GetEasingProgress() const { return m_tween->GetEasingProgress(); }
	// 素の経過時間を取得
	float GetLinearProgress() const { return m_tween->GetLinearProgress(); }

	// パラメータを設定
	void SetParam(const Tween3D::TweenParams& start, const Tween3D::TweenParams& delta);
	// イージングの種類を設定
	void SetEaseType(const Easing::EaseType type);

// 内部実装
private:

};
