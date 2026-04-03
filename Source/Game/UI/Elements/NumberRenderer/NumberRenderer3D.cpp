/**
 * @file   NumberRenderer3D.cpp
 *
 * @brief  3Dのスプライト数字に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NumberRenderer3D.h"

// 頂点位置・各頂点のUV座標
const DirectX::VertexPositionTexture NumberRenderer3D::VERTECES[4] =
{
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(0.5f,  0.5f, 0.0f),  DirectX::SimpleMath::Vector2(1.0f, 0.0f)),   // 0:左上
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(0.5f, -0.5f, 0.0f),  DirectX::SimpleMath::Vector2(1.0f, 1.0f)),   // 1:右上
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(-0.5f, -0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)),   // 3:右下
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(-0.5f,  0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)),   // 2:左下
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param mode			表示形式
 * @param spriteSize	スプライトのサイズ
 * @param texture		テクスチャのポインタ
 * @param digit			表示桁数
 * @param pDR			デバイスリソースのポインタ
 * @param boardScale	スケール
 */
NumberRenderer3D::NumberRenderer3D(
	DisplayMode mode,
	const DirectX::SimpleMath::Vector2& spriteSize,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
	int digit,
	DX::DeviceResources* pDR,
	float boardScale)
	:
	INumberRenderer(mode, spriteSize, texture, digit),
	DIGITS_WIDTH{ spriteSize.x * digit },
	m_scale{ boardScale },
	m_position{ DirectX::SimpleMath::Vector3::Zero },
	m_isBillboard{ false },
	m_billboard{ DirectX::SimpleMath::Matrix::Identity }
{
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext* context = pDR->GetD3DDeviceContext();

	// デバイスリソースのポインタを設定
	m_pDR = pDR;

	//エフェクトの作成
	m_batchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	//入力レイアウトの生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf()
	);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);

	// レンダーテクスチャの作成
	CreateRenderTexture();
}

/**
 * @brief デストラクタ
 */
NumberRenderer3D::~NumberRenderer3D()
{
	m_primitiveBatch.reset();
	m_batchEffect.reset();
	m_inputLayout.Reset();
}

/**
 * @brief 初期化処理
 *
 * @param number	表示数字
 *
 * @return なし
 */
void NumberRenderer3D::Initialize(const int& number)
{
	m_number = number;
}

/**
 * @brief 描画処理
 *
 * @param renderContext	描画用構造体
 *
 * @return なし
 */
void NumberRenderer3D::Draw(const RenderContext& renderContext)
{
	// オフスクリーンに数字を描画
	m_renderTexture->SetRTVTexture(renderContext.deviceContext, nullptr);
	const float clear[4] = { 0,0,0,0 };
	m_renderTexture->Clear(renderContext.deviceContext, clear);

	DirectX::SimpleMath::Vector2 size = SPRITE_SIZE * m_scale;

	int data = m_number;

	// スプライトサイズの半分
	DirectX::SimpleMath::Vector2 halfSize = SPRITE_SIZE * 0.5f;

	// 描画位置
	float x = 0.0f;
	float y = 0.0f;

	renderContext.spriteBatch->Begin();

	switch (m_mode)
	{
	case INumberRenderer::DisplayMode::Default:		// 通常
		DrawNumber(
			data, x, y, m_scale,
			DirectX::SimpleMath::Vector2::Zero,
			renderContext.spriteBatch);
		break;
	case INumberRenderer::DisplayMode::Fraction:	// 分数
		// 分子を描画
		x += (NUM_DIGIT - 1) * SPRITE_SIZE.x * m_scale;
		DrawNumber(
			m_fraction.numerator, x, y, m_scale,
			DirectX::SimpleMath::Vector2::Zero,
			renderContext.spriteBatch);

		// スラッシュを描画
		x += SPRITE_SIZE.x * m_scale;
		DrawSlash(
			x, y, m_scale,
			DirectX::SimpleMath::Vector2::Zero,
			renderContext.spriteBatch);

		// 表示位置を計算
		x += NUM_DIGIT * SPRITE_SIZE.x * m_scale;
		// 分母を描画
		DrawNumber(
			m_fraction.denominator, x, y, m_scale,
			DirectX::SimpleMath::Vector2::Zero,
			renderContext.spriteBatch);
		break;
	}

	renderContext.spriteBatch->End();

	// 通常の画面に切り替え
	m_renderTexture->SetRTVDefault(renderContext.deviceContext, nullptr);

	// 画像の幅と高さを取得
	float width = static_cast<float>(m_renderTexture->GetWidth());
	float height = static_cast<float>(m_renderTexture->GetHeight());
	// アスペクト比
	float aspect = width / height;

	// 頂点情報
	DirectX::VertexPositionTexture vertex[4]{};
	for (int j = 0; j < 4; j++)
	{
		vertex[j] = VERTECES[j];

		// 位置の設定
		vertex[j].position.x *= aspect;
		//vertex[j].position.y += m_position.y;
	}

	// テクスチャサンプラーの設定
	ID3D11SamplerState* samplers[1] = { renderContext.states->LinearClamp() };
	renderContext.deviceContext->PSSetSamplers(0, 1, samplers);

	//	半透明描画指定
	ID3D11BlendState* blendstate = renderContext.states->Opaque();

	//	透明判定処理
	renderContext.deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	// 深度テストの無効化
	renderContext.deviceContext->OMSetDepthStencilState(renderContext.states->DepthRead(), 0);

	// カリングの設定
	renderContext.deviceContext->RSSetState(renderContext.states->CullNone());

	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_scale);

	// ビルボードの回転
	if (m_isBillboard)
	{
		world *= m_billboard;
	}

	// 不透明のみ描画する設定
	m_batchEffect->SetTexture(m_renderTexture->GetSRV());
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);
	m_batchEffect->SetWorld(world);
	m_batchEffect->SetView(renderContext.view);
	m_batchEffect->SetProjection(renderContext.proj);
	m_batchEffect->Apply(renderContext.deviceContext);
	renderContext.deviceContext->IASetInputLayout(m_inputLayout.Get());

	//	半透明部分が含まれることを想定した描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_primitiveBatch->End();

	// 深度テストの設定をもとに戻す
	renderContext.deviceContext->OMSetDepthStencilState(renderContext.states->DepthDefault(), 0);

	// オフスクリーンのテスト
	//renderContext.spriteBatch->Begin();
	//renderContext.spriteBatch->Draw(m_renderTexture->GetSRV(), DirectX::SimpleMath::Vector2(0, 0));
	//renderContext.spriteBatch->End();
}

/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void NumberRenderer3D::Finalize()
{
	m_texture = nullptr;
}

/**
 * @brief ビルボードの作成
 *
 * @param eye	カメラ位置
 * @param up	カメラ上ベクトル
 *
 * @return なし
 */
void NumberRenderer3D::CreateBillboard(
	const DirectX::SimpleMath::Vector3& eye,
	const DirectX::SimpleMath::Vector3& up)
{
	//ビルボード状態にする
	m_isBillboard = true;

	// 行列を作成
	m_billboard = DirectX::SimpleMath::Matrix::CreateBillboard(m_position, eye, up);

	//Y軸を180度回転させる
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::Identity;
	rotY._11 = -1.0f;
	rotY._33 = -1.0f;

	//ビルボードを回転させる
	m_billboard = rotY * m_billboard;
}

/**
 * @brief 表示形式を設定
 *
 * @param mode 表示形式
 *
 * @return なし
 */
void NumberRenderer3D::SetDisplayMode(INumberRenderer::DisplayMode mode)
{
	// モードを設定
	m_mode = mode;

	// レンダーテクスチャを作成
	CreateRenderTexture();
}

/**
 * @brief レンダーテクスチャの作成
 *
 * @param なし
 *
 * @return なし
 */
void NumberRenderer3D::CreateRenderTexture()
{
	// レンダーテクスチャの幅を設定
	int width = 0;
	switch (m_mode)
	{
	case INumberRenderer::DisplayMode::Default:
		width = static_cast<int>(DIGITS_WIDTH * m_scale);
		break;
	case INumberRenderer::DisplayMode::Fraction:
		width = static_cast<int>((DIGITS_WIDTH * 2 + SPRITE_SIZE.x) * m_scale);
		break;
	}

	// レンダーテクスチャの作成
	m_renderTexture = std::make_unique<RenderTexture>();
	m_renderTexture->Initialize(
		m_pDR->GetD3DDevice(),
		width, static_cast<int>(SPRITE_SIZE.y * m_scale),
		m_pDR->GetRenderTargetView(),
		m_pDR->GetDepthStencilView()
	);
}