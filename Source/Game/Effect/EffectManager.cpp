/**
 * @file   EffectManager.cpp
 *
 * @brief  エフェクトマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EffectManager.h"


// インプットレイアウトを設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> EffectManager::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,  sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param deviceResources デバイスリソースのポインタ
 * @param states		  共通ステートのポインタ
 */
EffectManager::EffectManager(DX::DeviceResources* deviceResources, DirectX::CommonStates* states)
	:
	m_pDeviceResources{ deviceResources },
	m_pCamera{ nullptr },
	m_vs{ nullptr },
	m_ps{ nullptr },
	m_gs{ nullptr }
{
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(m_pDeviceResources->GetD3DDeviceContext());
	m_states = states;

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

	m_vs = nullptr;
	m_ps = nullptr;
	m_gs = nullptr;

	m_batch.reset();
	m_CBuffer.Reset();
	m_states = nullptr;
}



/**
 * @brief シェーダーの読み込み
 *
 * @param shaderManager シェーダーマネージャー
 *
 * @return なし
 */
void EffectManager::LoadShaders(ShaderManager* shaderManager)
{
	// 頂点シェーダー
	shaderManager->CreateVS("effectVS", "Resources/Shaders/Particle/ParticleVS.cso", INPUT_LAYOUT);
	m_vs = shaderManager->GetVS("effectVS");

	// ピクセルシェーダー
	shaderManager->CreatePS("effectPS", "Resources/Shaders/Particle/ParticlePS.cso");
	m_ps = shaderManager->GetPS("effectPS");

	// ジオメトリシェーダー
	shaderManager->CreateGS("effectGS", "Resources/Shaders/Particle/ParticleGS.cso");
	m_gs = shaderManager->GetGS("effectGS");

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_pDeviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_CBuffer);
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
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
	m_circle.erase(std::remove_if(m_circle.begin(), m_circle.end(),
		[&](std::unique_ptr<CircleParticleData>& t)
		{
			return !t->effect->IsActive();
		}),
		m_circle.end());

	// 軌跡エフェクトの更新
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Update(elapsedTime, *trajectory->position, trajectory->spawn, trajectory->random);
		trajectory->effect->CreateBillboard(m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp(), m_pCamera->GetForward());
	}

	// 円形エフェクトの更新
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Update(elapsedTime);
		circle->effect->CreateBillboard(m_pCamera->GetTarget(), m_pCamera->GetEye(), m_pCamera->GetUp(), m_pCamera->GetForward());
	}
}


/**
 * @brief 描画処理
 *
 * @param proj 射影行列
 *
 * @return なし
 */
void EffectManager::Draw(const DirectX::SimpleMath::Matrix& proj)
{
	// 軌跡エフェクトの描画
	for (std::unique_ptr<TrajectoryParticleData>& trajectory : m_trajectory)
	{
		trajectory->effect->Draw(m_pCamera->GetView(), proj);
	}

	// 円形エフェクトの描画
	for (std::unique_ptr<CircleParticleData>& circle : m_circle)
	{
		circle->effect->Draw(m_pCamera->GetView(), proj);
	}
}


/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void EffectManager::Finalize()
{
	m_trajectory.clear();
	m_circle.clear();

	m_pDeviceResources = nullptr;
	m_pCamera = nullptr;

	m_vs = nullptr;
	m_ps = nullptr;
	m_gs = nullptr;

	m_batch.reset();
	m_CBuffer.Reset();
	m_states = nullptr;
}


/**
 * @brief 軌跡エフェクトの生成
 *
 * @param texture		テクスチャのポインタ
 * @param particleData	パーティクルのパラメータ一覧
 * @param position		出現座標のポインタ
 * @param random		出現位置のランダム性
 *
 * @return 軌跡エフェクトのポインタ
 */
EffectManager::TrajectoryParticleData* EffectManager::CreateTrajectory(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, const ParticleUtility::ParticleData& particleData,
	DirectX::SimpleMath::Vector3* position, bool random)
{
	// 軌跡エフェクトの作成
	std::unique_ptr<TrajectoryParticle> effect = std::make_unique<TrajectoryParticle>();
	effect->Create(
		m_pDeviceResources,
		m_CBuffer.Get(),
		m_vs->inputLayout.Get(),
		m_batch.get(),
		m_states,
		texture,
		m_vs->vs.Get(),
		m_ps->ps.Get(),
		m_gs->gs.Get(),
		particleData
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
 * @param texture		テクスチャのポインタ
 * @param particleData	パーティクルのパラメータ一覧
 * @param position		出現座標のポインタ
 * @param range			円の半径
 * @param num			1度に出現する数
 * @param random		出現位置のランダム性
 * @param horizontal	円の向き
 *
 * @return 円形エフェクトのポインタ
 */
EffectManager::CircleParticleData* EffectManager::CreateCircle(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, const ParticleUtility::ParticleData& particleData,
	DirectX::SimpleMath::Vector3* position, float range, int num, bool random, bool horizontal)
{
	// 円形エフェクトの作成
	std::unique_ptr<CircleParticle> effect = std::make_unique<CircleParticle>();
	effect->Create(
		m_pDeviceResources, 
		m_CBuffer.Get(),
		m_vs->inputLayout.Get(),
		m_batch.get(),
		m_states,
		texture,
		m_vs->vs.Get(),
		m_ps->ps.Get(),
		m_gs->gs.Get(),
		particleData
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
