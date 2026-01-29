/**
 * @file   Background.cpp
 *
 * @brief  Backgroundに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Background.h"


// インプットレイアウトを設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> Background::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
Background::Background(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM,
	const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
	const DirectX::SimpleMath::Vector2& windowSize, const DirectX::SimpleMath::Vector2& moveDir)
	:
	m_windowSize(windowSize),
	m_moveDirection(moveDir),
	m_vs(nullptr),
	m_ps(nullptr),
	m_gs(nullptr),
	m_texture(nullptr),
	m_timer(0.0f)
{
	// プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pDR->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_CBuffer);

	// シェーダーの読み込み
	LoadShader(pSM, vs, ps, gs);

	// テクスチャの読み込み
	LoadTexture(pRM, tex);
}



/**
 * @brief デストラクタ
 */
Background::~Background()
{
	m_CBuffer.Reset();
	m_batch.reset();
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void Background::Update(float elapsedTime)
{
	m_timer += elapsedTime;
}



/**
 * @brief 描画処理
 *
 * @param なし
 *
 * @return なし
 */
void Background::Draw(const RenderContext& context)
{
	// 別名
	using matrix = DirectX::SimpleMath::Matrix;

	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(
			DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			DirectX::SimpleMath::Vector4::One,
			DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	//	シェーダーに渡す追加のバッファを作成
	ConstBuffer cbuff;
	cbuff.matView = matrix::Identity;
	cbuff.matProj = matrix::Identity;
	cbuff.matWorld = matrix::Identity;
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.WindowSize = m_windowSize;
	cbuff.MoveDir = m_moveDirection;
	cbuff.Time = m_timer;


	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context.deviceContext->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context.deviceContext->VSSetConstantBuffers(0, 1, cb);
	context.deviceContext->GSSetConstantBuffers(0, 1, cb);
	context.deviceContext->PSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { context.states->LinearWrap() };
	context.deviceContext->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = context.states->NonPremultiplied();

	//	透明判定処理
	context.deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context.deviceContext->OMSetDepthStencilState(context.states->DepthDefault(), 0);

	//	カリングは左周り
	context.deviceContext->RSSetState(context.states->CullNone());

	//	シェーダをセットする
	context.deviceContext->VSSetShader(m_vs->vs.Get(), nullptr, 0);
	context.deviceContext->PSSetShader(m_ps->ps.Get(), nullptr, 0);
	context.deviceContext->GSSetShader(m_gs->gs.Get(), nullptr, 0);

	//	ピクセルシェーダにテクスチャを登録する。
	context.deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

	//	インプットレイアウトの登録
	context.deviceContext->IASetInputLayout(m_vs->inputLayout.Get());

	//	板ポリゴンを描画
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	m_batch->End();

	//	シェーダの登録を解除しておく
	context.deviceContext->VSSetShader(nullptr, nullptr, 0);
	context.deviceContext->PSSetShader(nullptr, nullptr, 0);
	context.deviceContext->GSSetShader(nullptr, nullptr, 0);
}



/**
 * @brief シェーダーの読み込み
 *
 * @param pSM	シェーダーマネージャーのポインタ
 * @param vs	頂点シェーダーのキー・ファイルパス
 * @param ps	ピクセルシェーダーのキー・ファイルパス
 * @param gs	ジオメトリシェーダーのキー・ファイルパス
 *
 * @return なし
 */
void Background::LoadShader(
	ShaderManager* pSM, 
	const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs)
{
	// 頂点シェーダー
	pSM->CreateVS(vs.key, vs.path, INPUT_LAYOUT);
	m_vs = pSM->GetVS(vs.key);

	// ピクセルシェーダー
	pSM->CreatePS(ps.key, ps.path);
	m_ps = pSM->GetPS(ps.key);

	// ジオメトリシェーダー
	pSM->CreateGS(gs.key, gs.path);
	m_gs = pSM->GetGS(gs.key);
}



/**
 * @brief テクスチャの読み込み
 *
 * @param pRM	リソースマネージャーのポインタ
 * @param desc	画像のキー・ファイルパス
 *
 * @return なし
 */
void Background::LoadTexture(ResourceManager* pRM, const ResourcesDesc& desc)
{
	m_texture = pRM->RequestPNG(desc.key, desc.path);
}
