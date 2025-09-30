/**
 * @file   NumberRenderer3D.cpp
 *
 * @brief  NumberRenderer3Dに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "NumberRenderer3D.h"

using namespace DirectX;


// 頂点座標・各頂点のUV座標
const VertexPositionTexture NumberRenderer3D::VERTECES[4] =
{
	VertexPositionTexture(SimpleMath::Vector3( 0.5f,  0.5f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f)),   // 0:左上
	VertexPositionTexture(SimpleMath::Vector3( 0.5f, -0.5f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f)),   // 1:右上
	VertexPositionTexture(SimpleMath::Vector3(-0.5f, -0.5f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f)),   // 3:右下
	VertexPositionTexture(SimpleMath::Vector3(-0.5f,  0.5f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f)),   // 2:左下
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
NumberRenderer3D::NumberRenderer3D(
	DirectX::SimpleMath::Vector2 spriteSize,
	ID3D11ShaderResourceView* texture,
	int digit,
	DX::DeviceResources* deviceResources)
	: INumberRenderer(spriteSize, texture, digit)
	, DIGITS_WIDTH{ spriteSize.x * digit }
	, m_position{ SimpleMath::Vector3::Zero }
	, m_isBillboard{ false }
	, m_billboard{ SimpleMath::Matrix::Identity }
{
	ID3D11Device1* device = deviceResources->GetD3DDevice();
	ID3D11DeviceContext* context = deviceResources->GetD3DDeviceContext();

	//エフェクトの作成
	m_batchEffect = std::make_unique<AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	//入力レイアウトの生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf()
	);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// レンダーテクスチャの作成
	m_renderTexture = std::make_unique<RenderTexture>();
	m_renderTexture->Initialize(deviceResources->GetD3DDevice(), 100, 100, deviceResources->GetRenderTargetView());
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
 * @param[in] なし
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
 * @param[in] なし
 * 
 * @return なし
 */
void NumberRenderer3D::Draw(RenderContext& renderContext)
{
	// オフスクリーンに数字を描画
	m_renderTexture->SetRTVTexture(renderContext.deviceContext, nullptr);
	const float clear[4] = { 0,0,0,0 };
	m_renderTexture->Clear(renderContext.deviceContext, clear);

	int data = m_number;
	float x = m_position.x + NUM_DIGIT * SPRITE_SIZE.x;
	float y = m_position.y;

	renderContext.spriteBatch->Begin();

	for (int i = 0; i < NUM_DIGIT; i++)
	{
		int number = data % 10;
		int sourceX = number * SPRITE_SIZE.x;
		DirectX::SimpleMath::Vector2 pos = { x,y };
		RECT rect = { sourceX,0,sourceX + SPRITE_SIZE.x,SPRITE_SIZE.y };
		DirectX::FXMVECTOR color = DirectX::Colors::White;

		renderContext.spriteBatch->Draw(m_texture, pos, &rect,
			color, 0.0f, DirectX::XMFLOAT2(0, 0),
			1.0f, DirectX::SpriteEffects_None, 0.0f);

		data /= 10;
		x -= SPRITE_SIZE.x;
	}

	renderContext.spriteBatch->End();

	// 通常の画面に切り替え
	m_renderTexture->SetRTVDefault(renderContext.deviceContext, nullptr);

	// テクスチャサンプラーの設定
	ID3D11SamplerState* samplers[1] = { renderContext.states->PointClamp() };
	renderContext.deviceContext->PSSetSamplers(0, 1, samplers);

	// 深度バッファに書き込み参照する
	renderContext.deviceContext->OMSetDepthStencilState(renderContext.states->DepthDefault(), 0);

	// カリングの設定
	renderContext.deviceContext->RSSetState(renderContext.states->CullNone());

	// ワールド行列の計算
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;

	// 回転
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

	// 頂点情報
	VertexPositionTexture vertex[4];
	for (int j = 0; j < 4; j++)
	{
		vertex[j] = VERTECES[j];

		// 座標の設定
		//vertex[j].position.x = (m_position.x + 100.f) * vertex[j].position.x;
		vertex[j].position.x += m_position.x;
		vertex[j].position.y += m_position.y;
		vertex[j].position.z += m_position.z;
	}

	m_primitiveBatch->Begin();

	// 半透明部分が含まれることを想定した描画
	m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);

	m_primitiveBatch->End();

	m_isBillboard = false;
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void NumberRenderer3D::Finalize()
{
	m_texture = nullptr;
}



void NumberRenderer3D::CreateBillboard(
	DirectX::SimpleMath::Vector3 eye,
	DirectX::SimpleMath::Vector3 up)
{
	//ビルボード状態にする
	m_isBillboard = true;

	// 行列を作成
	m_billboard = DirectX::SimpleMath::Matrix::CreateBillboard(
		m_position,
		eye,
		up
	);

	//Y軸を180度回転させる
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::Identity;
	rotY._11 = -1.0f;
	rotY._33 = -1.0f;

	//ビルボードを回転させる
	m_billboard = rotY * m_billboard;
}
