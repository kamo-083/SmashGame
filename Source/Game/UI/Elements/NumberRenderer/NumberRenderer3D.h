/**
 * @file   NumberRenderer3D.h
 *
 * @brief  3Dのスプライト数字に関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"Source/Game/UI/Elements/NumberRenderer/INumberRenderer.h"
#include"Source/Game/Common/DeviceResources.h"
#include"Source/Game/Common/RenderTexture.h"

// クラスの定義 ===============================================================
/**
 * @brief 3Dのスプライト数字
 */
class NumberRenderer3D :public INumberRenderer
{
// クラス定数の宣言 -------------------------------------------------
public:
	// 頂点の配列
	const static DirectX::VertexPositionTexture VERTECES[4];

	// 表示する数値全体の幅
	const float DIGITS_WIDTH;

// データメンバの宣言 -----------------------------------------------
private:
	// デバイスリソースのポインタ
	DX::DeviceResources* m_pDR;

	// 表示座標
	DirectX::SimpleMath::Vector3 m_position;

	// 大きさ
	float m_scale;

	// ビルボード関連
	bool m_isBillboard;
	DirectX::SimpleMath::Matrix m_billboard;

	//入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//エフェクト
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;

	//プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// レンダーテクスチャ
	std::unique_ptr<RenderTexture> m_renderTexture;

// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NumberRenderer3D(
		DisplayMode mode,
		const DirectX::SimpleMath::Vector2& spriteSize,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		int digit,
		DX::DeviceResources* pDR,
		float boardScale = 1.f);

	// デストラクタ
	~NumberRenderer3D();

// 操作
public:
	// 初期化処理
	void Initialize(const int& number) override;

	// 描画処理
	void Draw(const RenderContext& renderContext) override;

	// 終了処理
	void Finalize() override;

	// ビルボード行列を作成
	void CreateBillboard(
		const DirectX::SimpleMath::Vector3& eye,
		const DirectX::SimpleMath::Vector3& up
	);

// 取得/設定
public:
	// 座標を設定
	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { m_position = pos; }

	// スケールを取得
	float GetScale() const { return m_scale; }

	// 表示形式を設定
	void SetDisplayMode(INumberRenderer::DisplayMode mode) override;

// 内部実装
private:
	// レンダーテクスチャの作成
	void CreateRenderTexture();
};