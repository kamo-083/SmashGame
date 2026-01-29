/**
 * @file   BlockTransition.cpp
 *
 * @brief  シーン遷移演出に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BlockTransition.h"
#include "Source/Game/Common/DeviceResources.h"


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
BlockTransition::BlockTransition(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM,
	const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
	const DirectX::SimpleMath::Vector2& windowSize, float interval)	
	: ITransition(pDR, pSM, pRM, vs, ps, gs, tex, windowSize, interval)
{
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
 * @brief デストラクタ
 */
BlockTransition::~BlockTransition()
{

}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void BlockTransition::Update(float elapsedTime)
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
void BlockTransition::Draw(const RenderContext& context)
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

	//	シェーダーに渡す追加のバッファを作成
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

	//	ピクセルシェーダにテクスチャを登録する
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
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void BlockTransition::Finalize()
{

}