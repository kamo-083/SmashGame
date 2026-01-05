/**
 * @file   AreaEffect.cpp
 *
 * @brief  エリアのエフェクトに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "AreaEffect.h"

// インプットレイアウトの設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> AreaEffect::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR	ユーザーリソースのポインタ
 * @param desc	初期設定の引数群
 */
AreaEffect::AreaEffect(UserResources* pUR, SetupDesc desc)
	: m_timer(0.0f)
{
	DX::DeviceResources* dr = pUR->GetDeviceResources();

	// バッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(dr->GetD3DDeviceContext());

	// シェーダーの読み込み
	LoadShaders(pUR->GetShaderManager(), dr->GetD3DDevice());

	// 各値の初期化
	m_position = desc.position;	// 中心位置
	m_length = desc.length;		// 各辺の長さ
	m_color = desc.color;		// 色
}

/**
 * @brief コンストラクタ
 *
 * @param pUR	ユーザーリソースのポインタ
 */
AreaEffect::AreaEffect(UserResources* pUR)
	: m_timer(0.0f)
{
	DX::DeviceResources* dr = pUR->GetDeviceResources();

	// バッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(dr->GetD3DDeviceContext());

	// シェーダーの読み込み
	LoadShaders(pUR->GetShaderManager(), dr->GetD3DDevice());
}



/**
 * @brief デストラクタ
 */
AreaEffect::~AreaEffect()
{
	m_vs = nullptr;
	m_ps = nullptr;
	m_gs = nullptr;
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void AreaEffect::Update(float elapsedTime)
{
	// 累計経過時間の更新
	m_timer += elapsedTime;
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void AreaEffect::Draw(RenderContext& context)
{
	ConstBuffer cbuff;
	cbuff.matView = context.view.Transpose();
	cbuff.matProj = context.proj.Transpose();
	cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.Height = m_length.y;
	cbuff.Time = m_timer;
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };

	context.deviceContext->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	context.deviceContext->VSSetConstantBuffers(0, 1, cb);
	context.deviceContext->PSSetConstantBuffers(0, 1, cb);
	context.deviceContext->GSSetConstantBuffers(0, 1, cb);

	// 半透明描画指定
	ID3D11BlendState* blendstate = context.states->NonPremultiplied();

	// 透明判定処理
	context.deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	// 深度バッファは読み取り専用
	context.deviceContext->OMSetDepthStencilState(context.states->DepthRead(), 0);

	// カリングはなし
	context.deviceContext->RSSetState(context.states->CullNone());

	// シェーダをセットする
	context.deviceContext->VSSetShader(m_vs->vs.Get(), nullptr, 0);
	context.deviceContext->PSSetShader(m_ps->ps.Get(), nullptr, 0);
	context.deviceContext->GSSetShader(m_gs->gs.Get(), nullptr, 0);

	// インプットレイアウトの登録
	context.deviceContext->IASetInputLayout(m_vs->inputLayout.Get());

	DirectX::SimpleMath::Vector3 size = m_length;
	size *= 0.5f;

	float vertexes[4];
	vertexes[0] = m_position.z + size.z;	// 奥
	vertexes[1] = m_position.x + size.x;	// 右
	vertexes[2] = m_position.z - size.z;	// 手前
	vertexes[3] = m_position.x - size.x;	// 左

	DirectX::VertexPositionColor v[2];

	// 板ポリゴンを描画
	m_batch->Begin();

	// 奥
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], m_position.y, vertexes[0]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], m_position.y, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 右
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[1], m_position.y, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], m_position.y, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 左
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], m_position.y, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[3], m_position.y, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 手前
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], m_position.y, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], m_position.y, vertexes[2]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	m_batch->End();

	// シェーダの登録を解除
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
void AreaEffect::Finalize()
{

}



/**
 * @brief シェーダーの読み込み
 *
 * @param shaderManager シェーダーマネージャー
 * @param device デバイスのポインタ
 *
 * @return なし
 */
void AreaEffect::LoadShaders(ShaderManager* shaderManager, ID3D11Device* device)
{
	// 頂点シェーダー
	shaderManager->CreateVS("areaVS", "Shaders/Area/AreaVS.cso", INPUT_LAYOUT);
	m_vs = shaderManager->GetVS("areaVS");

	// ピクセルシェーダー
	shaderManager->CreatePS("areaPS", "Shaders/Area/AreaPS.cso");
	m_ps = shaderManager->GetPS("areaPS");

	// ジオメトリシェーダー
	shaderManager->CreateGS("areaGS", "Shaders/Area/AreaGS.cso");
	m_gs = shaderManager->GetGS("areaGS");

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}
