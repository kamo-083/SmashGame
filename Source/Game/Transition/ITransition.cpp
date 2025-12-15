/**
 * @file   ITransition.cpp
 *
 * @brief  シーン遷移演出に関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ITransition.h"
#include "Source/Game/Common/DeviceResources.h"

// インプットレイアウトを設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> ITransition::INPUT_LAYOUT =
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
 * @param vs		 頂点シェーダーのキー・ファイルパス
 * @param ps		 ピクセルシェーダーのキー・ファイルパス
 * @param gs		 ジオメトリシェーダーのキー・ファイルパス
 * @param tex		 画像のキー・ファイルパス
 * @param windowSize ウィンドウサイズ
 * @param interval	 開閉にかける時間
 */
ITransition::ITransition(
	DX::DeviceResources* pDR,
	ShaderManager* pSM,
	ResourceManager* pRM,
	const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
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
	LoadShader(pDR, pSM, vs, ps, gs);

	// テクスチャの読み込み
	LoadTexture(pRM, tex);
}



/**
 * @brief デストラクタ
 */
ITransition::~ITransition()
{
	m_CBuffer.Reset();
	m_batch.reset();
}



/**
 * @brief 開く
 *
 * @param なし
 *
 * @return なし
 */
void ITransition::Open()
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
void ITransition::Close()
{
	m_open = false;
	m_rate = 0.0f;
}



/**
 * @brief シェーダーの読み込み
 *
 * @param pDR	デバイスリソースのポインタ
 * @param pSM	シェーダーマネージャーのポインタ
 * @param vs	頂点シェーダーのキー・ファイルパス
 * @param ps	ピクセルシェーダーのキー・ファイルパス
 * @param gs	ジオメトリシェーダーのキー・ファイルパス
 *
 * @return なし
 */
void ITransition::LoadShader(
	DX::DeviceResources* pDR, ShaderManager* pSM,
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
void ITransition::LoadTexture(ResourceManager* pRM, const ResourcesDesc& desc)
{
	m_texture = pRM->RequestPNG(desc.key, desc.path);
}



/**
 * @brief 終了しているかを返す
 *
 * @param なし
 *
 * @return 終了しているか
 */
bool ITransition::IsEnd()
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
float ITransition::GetRate()
{
	if (m_open) return 1.0f - m_rate;
	return m_rate;
}
