/**
 * @file   UIDimmer.cpp
 *
 * @brief  UIで使用するディマーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIDimmer.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pDR デバイスリソースのポインタ
 */
UIDimmer::UIDimmer(DX::DeviceResources* pDR)
{
	// プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pDR->GetD3DDeviceContext());

	// ベーシックエフェクトの作成
	ID3D11Device* device = pDR->GetD3DDevice();
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	m_basicEffect->SetLightingEnabled(false);
	m_basicEffect->SetVertexColorEnabled(true);
	m_basicEffect->SetTextureEnabled(false);

	// 入力レイアウトの作成
	DX::ThrowIfFailed(
		DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionColor>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);

	// 頂点データの作成
	CreateVertexes(pDR->GetOutputSize(), 0.0f);
}

/**
 * @brief デストラクタ
 */
UIDimmer::~UIDimmer()
{
	m_batch.reset();
}

/**
 * @brief 描画処理
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void UIDimmer::Draw(const RenderContext& context)
{
	// エフェクトを設定
	context.deviceContext->OMSetBlendState(context.states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	context.deviceContext->OMSetDepthStencilState(context.states->DepthNone(), 0);
	context.deviceContext->RSSetState(context.states->CullNone());
	m_basicEffect->Apply(context.deviceContext);
	context.deviceContext->IASetInputLayout(m_inputLayout.Get());

	// 描画
	m_batch->Begin();
	m_batch->DrawQuad(m_vertexes[0], m_vertexes[1], m_vertexes[2], m_vertexes[3]);
	m_batch->End();
}

/**
 * @brief 不透明度の設定 (値を直接設定)
 *
 * @param opacity	不透明度
 *
 * @return なし
 */
void UIDimmer::SetOpacity(float opacity)
{
	// 各頂点に不透明度を設定
	for (DirectX::VertexPositionColor& v : m_vertexes)
	{
		v.color.w = std::min(std::max(opacity, 0.0f), 1.0f);	// ０～１に収める
	}
}

/**
 * @brief 不透明度の設定 (イージングを使用)
 *
 * @param time		 経過時間
 * @param type		 イージングの種類
 * @param maxOpacity 不透明度の最大値
 *
 * @return なし
 */
void UIDimmer::SetOpacity(float time, Easing::EaseType type, float maxOpacity)
{
	// 不透明度を計算
	float opacity = maxOpacity * Easing::EaseValue(type, time);
	// ０～１に収める
	opacity = std::min(std::max(opacity, 0.0f), maxOpacity);

	// 各頂点に不透明度を設定
	for (DirectX::VertexPositionColor& v : m_vertexes)
	{
		v.color.w = opacity;
	}
}

/**
 * @brief 頂点データの作成
 *
 * @param windowSize ウィンドウサイズ
 * @param opacity	 不透明度
 *
 * @return なし
 */
void UIDimmer::CreateVertexes(const RECT& windowSize, float opacity)
{
	// ウィンドウサイズの半分を計算
	DirectX::SimpleMath::Vector2 halfSize =
	{
		static_cast<float>(windowSize.right),
		static_cast<float>(windowSize.bottom) };

	// 頂点データの作成
	m_vertexes.resize(4);
	m_vertexes[0] = {
		DirectX::SimpleMath::Vector3(-halfSize.x, -halfSize.y, 0),
		DirectX::SimpleMath::Color(0, 0, 0, opacity)
	};
	m_vertexes[1] = {
		DirectX::SimpleMath::Vector3(halfSize.x, -halfSize.y, 0),
		DirectX::SimpleMath::Color(0, 0, 0, opacity)
	};
	m_vertexes[2] = {
		DirectX::SimpleMath::Vector3(halfSize.x, halfSize.y, 0),
		DirectX::SimpleMath::Color(0, 0, 0, opacity)
	};
	m_vertexes[3] = {
		DirectX::SimpleMath::Vector3(-halfSize.x, halfSize.y, 0),
		DirectX::SimpleMath::Color(0, 0, 0, opacity)
	};
}