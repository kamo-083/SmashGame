/**
 * @file   EffectManager.cpp
 *
 * @brief  エフェクトマネージャーに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EffectManager.h"
#include "Source/Game/Common/BinaryFile.h"

using namespace DirectX;


const std::vector<D3D11_INPUT_ELEMENT_DESC> EffectManager::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(SimpleMath::Vector3) + sizeof(SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] deviceResources デバイスリソースのポインタ
 * @param[in] states		  共通ステートのポインタ
 */
EffectManager::EffectManager(DX::DeviceResources* deviceResources, DirectX::CommonStates* states)
	: m_pDeviceResources{ deviceResources }
	, m_pCamera{ nullptr }
{
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(m_pDeviceResources->GetD3DDeviceContext());
	m_states = states;

	CreateShader();

	// -- debug ----------------
	//static const char c_szName[] = "EffectManager";
	//HRESULT hr = m_pDeviceResources->GetD3DDeviceContext()->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
	auto* alpha = m_states->AlphaBlend();
	alpha->SetPrivateData(WKPDID_D3DDebugObjectName, 26, "Effect.CommonStates.Alpha");
}


/**
 * @brief デストラクタ
 */
EffectManager::~EffectManager()
{
	m_trajectory.clear();
	m_circle.clear();

	m_pDeviceResources = nullptr;
	m_pCamera = nullptr;

	m_batch.reset();
	m_inputLayout.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_geometryShader.Reset();
	m_CBuffer.Reset();
	m_states = nullptr;
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void EffectManager::Update(float elapsedTime)
{
	// 無効なエフェクトを削除
	m_trajectory.erase(std::remove_if(m_trajectory.begin(), m_trajectory.end(),
		[&](std::unique_ptr<TrajectoryParticleData>& t)
		{
			return !t->effect->IsActive();
		}),
		m_trajectory.end());

	// 軌跡エフェクトの更新
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Update(elapsedTime, *trajectory->position, trajectory->spawn, trajectory->random);
		trajectory->effect->CreateBillboard(*trajectory->position, m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp());
	}

	// 円形エフェクトの更新
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Update(elapsedTime);
		circle->effect->CreateBillboard(*circle->position, m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp());
	}
}


/**
 * @brief 描画処理
 *
 * @param[in] proj 射影行列
 *
 * @return なし
 */
void EffectManager::Draw(DirectX::SimpleMath::Matrix proj)
{
	// 軌跡エフェクトの描画
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Render(m_pCamera->GetView(), proj);
	}

	// 円形エフェクトの描画
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Render(m_pCamera->GetView(), proj);
	}
}


/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::Finalize()
{
	m_trajectory.clear();
	m_circle.clear();

	m_pDeviceResources = nullptr;
	m_pCamera = nullptr;

	m_batch.reset();
	m_inputLayout.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_geometryShader.Reset();
	m_CBuffer.Reset();
	m_states = nullptr;
}


/**
 * @brief 軌跡エフェクトの生成
 *
 * @param[in] texture	テクスチャのポインタ
 * @param[in] scale		大きさ
 * @param[in] life		寿命
 * @param[in] color		色
 * @param[in] position	出現座標のポインタ
 * @param[in] random	出現位置のランダム性
 *
 * @return 軌跡エフェクトのポインタ
 */
EffectManager::TrajectoryParticleData* EffectManager::CreateTrajectory(
	ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color,
	DirectX::SimpleMath::Vector3* position, bool random)
{
	// 軌跡エフェクトの作成
	std::unique_ptr<TrajectoryParticle> effect = std::make_unique<TrajectoryParticle>();
	effect->Create(
		m_pDeviceResources,
		m_CBuffer.Get(),
		m_inputLayout.Get(),
		m_batch.get(),
		m_states,
		texture,
		m_vertexShader.Get(),
		m_pixelShader.Get(),
		m_geometryShader.Get(),
		scale, life, color
	);

	// パーティクルデータ構造体の作成
	std::unique_ptr<TrajectoryParticleData> data = std::make_unique<TrajectoryParticleData>
		(
			std::move(effect),
			position,
			false,
			random
		);

	// 配列に格納
	m_trajectory.push_back(std::move(data));

	// 作成したエフェクトのポインタを返す
	return m_trajectory.back().get();
}


/**
 * @brief 円形エフェクトの生成
 *
 * @param[in] texture	 テクスチャのポインタ
 * @param[in] scale		 大きさ
 * @param[in] life		 寿命
 * @param[in] color		 色
 * @param[in] position   出現座標のポインタ
 * @param[in] range		 円の半径
 * @param[in] num		 1度に出現する数
 * @param[in] random	 出現位置のランダム性
 * @param[in] horizontal 円の向き
 *
 * @return 円形エフェクトのポインタ
 */
EffectManager::CircleParticleData* EffectManager::CreateCircle(
	ID3D11ShaderResourceView* texture, float scale, float life, DirectX::SimpleMath::Color color, 
	DirectX::SimpleMath::Vector3* position, float range, int num, bool random, bool horizontal)
{
	// 円形エフェクトの作成
	std::unique_ptr<CircleParticle> effect = std::make_unique<CircleParticle>();
	effect->Create(
		m_pDeviceResources, 
		m_CBuffer.Get(),
		m_inputLayout.Get(),
		m_batch.get(),
		m_states,
		texture,
		m_vertexShader.Get(),
		m_pixelShader.Get(),
		m_geometryShader.Get(),
		scale, life, color
	);

	// パーティクルデータ構造体の作成
	std::unique_ptr<CircleParticleData> data = std::make_unique<CircleParticleData>
		(
			std::move(effect),
			position,
			range,
			num,
			random,
			horizontal
		);

	// 配列に格納
	m_circle.push_back(std::move(data));

	// 作成したエフェクトのポインタを返す
	return m_circle.back().get();
}


/**
 * @brief シェーダーを作成
 *
 * @param[in] なし
 *
 * @return なし
 */
void EffectManager::CreateShader()
{
	ID3D11Device1* device = m_pDeviceResources->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	std::unique_ptr<BinaryFile> VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	std::unique_ptr<BinaryFile> PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");
	std::unique_ptr<BinaryFile> GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");

	//	インプットレイアウトの作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
		VSData->GetData(), VSData->GetSize(),
		m_inputLayout.GetAddressOf());

	//	頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData->GetData(), VSData->GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData->GetData(), PSData->GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData->GetData(), GSData->GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}
