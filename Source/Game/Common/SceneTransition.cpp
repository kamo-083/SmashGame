/**
 * @file   SceneTransition.cpp
 *
 * @brief  シーン遷移演出に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SceneTransition.h"
#include "Source/Game/Common/DeviceResources.h"

// インプットレイアウトを設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> SceneTransition::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pDR		 デバイスリソースのポインタ
 * @param pSM		 シェーダーマネージャーのポインタ
 * @param pRM		 リソースマネージャーのポインタ
 * @param windowSize ウィンドウサイズ
 * @param interval	 開閉にかける時間
 */
SceneTransition::SceneTransition(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM, 
	DirectX::SimpleMath::Vector2 windowSize, float interval)
	: m_windowSize(windowSize)
	, m_interval(interval)
	, m_rate(0.0f)
	, m_open(true)
	, m_vs(nullptr)
	, m_ps(nullptr)
	, m_gs(nullptr)
	, m_texture(nullptr)
{
	// プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());
	
	// シェーダーの読み込み
	LoadShader(pDR, pSM);

	// テクスチャの読み込み
	LoadTexture(pRM);
}



/**
 * @brief デストラクタ
 */
SceneTransition::~SceneTransition()
{
	m_CBuffer.Reset();
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void SceneTransition::Update(float elapsedTime)
{
	if (m_open)
	{
		// オープン
		m_rate -= elapsedTime / m_interval;
		if (m_rate < 0.0f) m_rate = 0.0f;
	}
	else
	{
		// クローズ
		m_rate += elapsedTime / m_interval;
		if (m_rate > 1.0f) m_rate = 1.0f;
	}
}



/**
 * @brief 描画処理
 *
 * @param context 描画用構造体
 *
 * @return なし
 */
void SceneTransition::Draw(RenderContext context)
{
	if (m_rate == 0.0f) return;

	// 別名
	using matrix = DirectX::SimpleMath::Matrix;

	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(
			DirectX::SimpleMath::Vector3(0.0f,  0.0f, 0.0f),
			DirectX::SimpleMath::Vector4::One,
			DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	ConstBuffer cbuff;
	cbuff.matView = matrix::Identity;
	cbuff.matProj = matrix::Identity;
	cbuff.matWorld = matrix::Identity;
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.WindowSize = m_windowSize;
	cbuff.Rate = m_rate;

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
	context.deviceContext->PSSetShaderResources(0, 1, &m_texture);

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
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void SceneTransition::Finalize()
{

}



/**
 * @brief 開く
 *
 * @param なし
 *
 * @return なし
 */
void SceneTransition::Open()
{
	m_open = true;
	m_rate = 1.0f;
}



/**
 * @brief 閉じる
 *
 * @param なし
 *
 * @return なし
 */
void SceneTransition::Close()
{
	m_open = false;
	m_rate = 0.0f;
}



/**
 * @brief シェーダーの読み込み
 *
 * @param pDR	デバイスリソースのポインタ
 * @param pSM	シェーダーマネージャーのポインタ
 *
 * @return なし
 */
void SceneTransition::LoadShader(DX::DeviceResources* pDR, ShaderManager* pSM)
{
	// 頂点シェーダー
	pSM->CreateVS("transitionVS", L"Shaders/Transition/TransitionVS.cso", INPUT_LAYOUT);
	m_vs = pSM->GetVS("transitionVS");

	// ピクセルシェーダー
	pSM->CreatePS("transitionPS", L"Shaders/Transition/TransitionPS.cso");
	m_ps = pSM->GetPS("transitionPS");

	// ジオメトリシェーダー
	pSM->CreateGS("transitionGS", L"Shaders/Transition/TransitionGS.cso");
	m_gs = pSM->GetGS("transitionGS");

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pDR->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_CBuffer);
}



/**
 * @brief テクスチャの読み込み
 *
 * @param pRM	リソースマネージャーのポインタ
 *
 * @return なし
 */
void SceneTransition::LoadTexture(ResourceManager* pRM)
{
	m_texture = pRM->RequestPNG("transition", "Others/block_mask.png");
}



/**
 * @brief 終了しているかを返す
 *
 * @param なし
 *
 * @return 終了しているか
 */
bool SceneTransition::IsEnd()
{
	if (GetRate() == 1.0f) return true;
	else return false;
}



/**
 * @brief 進行割合を返す
 *
 * @param なし
 *
 * @return 進行割合
 */
float SceneTransition::GetRate()
{
	if (m_open) return 1.0f - m_rate;
	return m_rate;
}
