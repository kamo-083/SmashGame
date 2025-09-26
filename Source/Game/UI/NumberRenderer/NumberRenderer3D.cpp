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
 * 空の画像を内部で作って、数字の画像を貼って１つの画像にするみたいなやり方もある
 * 回転や拡縮してもいい感じになるらしい
 * 
 * @return なし
 */
void NumberRenderer3D::Draw(RenderContext& renderContext)
{
	// 数値を取得
	int data = m_number;

	// 表示するX座標を計算
	float pos_x = m_position.x + 1.f * 0.5f;

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
	m_batchEffect->SetTexture(m_texture);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);
	m_batchEffect->SetWorld(world);
	m_batchEffect->SetView(renderContext.view);
	m_batchEffect->SetProjection(renderContext.proj);
	m_batchEffect->Apply(renderContext.deviceContext);
	renderContext.deviceContext->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	for (int i = 0; i < NUM_DIGIT; i++)
	{
		// 表示値を計算
		int number = data % 10;

		// 頂点情報
		VertexPositionTexture vertex[4];
		for (int j = 0; j < 4; j++)
		{
			vertex[j] = VERTECES[j];

			// 座標の設定
			vertex[j].position.x += pos_x;
			vertex[j].position.y += m_position.y;
			vertex[j].position.z += m_position.z;
		}

		// UV座標の設定
		float sprite_width = SPRITE_SIZE.x * 10.0f;
		float uv_x = SPRITE_SIZE.x * number;
		vertex[0].textureCoordinate = { (uv_x + SPRITE_SIZE.x) / sprite_width, 0.0f };
		vertex[1].textureCoordinate = { (uv_x + SPRITE_SIZE.x) / sprite_width, 1.0f };
		vertex[2].textureCoordinate = { uv_x / sprite_width, 1.0f };
		vertex[3].textureCoordinate = { uv_x / sprite_width, 0.0f };

		// 半透明部分が含まれることを想定した描画
		m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);

		// 値を更新
		data /= 10;

		// X座標を更新
		pos_x -= 1.f;
	}

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
