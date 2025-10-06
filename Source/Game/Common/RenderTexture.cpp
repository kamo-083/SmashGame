/**
 * @file   RenderTexture.cpp
 *
 * @brief  RenderTextureに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "RenderTexture.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
RenderTexture::RenderTexture()
	: m_width(0)
	, m_height(0)
	, m_defaultRTV(nullptr)
	, m_defaultDSV(nullptr)
{

}



/**
 * @brief デストラクタ
 */
RenderTexture::~RenderTexture()
{
	m_defaultRTV = nullptr;
	m_defaultDSV = nullptr;
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
bool RenderTexture::Initialize(
	ID3D11Device* device,
	int width,
	int height,
	ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsv)
{
	HRESULT result;

	// レンダリングターゲットの設定
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// レンダーターゲットテクスチャの作成
	result = device->CreateTexture2D(&texDesc, nullptr, &m_renderTargetTexrute);
	if (FAILED(result))
	{
		// 失敗
		return false;
	}

	// レンダーターゲットビューを作成
	result = device->CreateRenderTargetView(m_renderTargetTexrute.Get(), nullptr, &m_renderTargetView);
	if (FAILED(result))
	{
		// 失敗
		return false;
	}

	// シェーダーリソースビューの作成
	result = device->CreateShaderResourceView(m_renderTargetTexrute.Get(), nullptr, &m_shaderResourceView);
	if (FAILED(result))
	{
		// 失敗
		return false;
	}

	// レンダリングテクスチャのサイズを格納
	m_width = width;
	m_height = height;

	// 通常のレンダーターゲットビューを格納
	m_defaultRTV = rtv;

	// 通常の深度ステンシルビューを格納
	m_defaultDSV = dsv;

	return true;
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void RenderTexture::Finalize()
{

}


void RenderTexture::SetRTVTexture(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView)
{
	ID3D11RenderTargetView* rtv = m_renderTargetView.Get();

	context->OMSetRenderTargets(1, &rtv, depthStencilView);
}

void RenderTexture::SetRTVDefault(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthStencilView)
{
	ID3D11RenderTargetView* rtv = m_defaultRTV;

	if (!depthStencilView) depthStencilView = m_defaultDSV;
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
}


void RenderTexture::Clear(ID3D11DeviceContext* context, const float color[4])
{
	context->ClearRenderTargetView(m_renderTargetView.Get(), color);
}
