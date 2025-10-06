//--------------------------------------------------------------------------------------
// File: TrajectoryParticle.cpp
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "TrajectoryParticle.h"

#include "Source/Game/Common/BinaryFile.h"
#include "Source/Game/Common/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <algorithm>
#include <random>

using namespace DirectX;

const std::vector<D3D11_INPUT_ELEMENT_DESC> TrajectoryParticle::INPUT_LAYOUT =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 0,														  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(SimpleMath::Vector3),								  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, sizeof(SimpleMath::Vector3) + sizeof(SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TrajectoryParticle::TrajectoryParticle()
	:m_pDR(nullptr)
	, m_CBuffer(nullptr)
	, m_inputLayout(nullptr)
	, m_batch(nullptr)
	, m_states(nullptr)
	, m_texture()
	, m_vertices()
	, m_vertexShader(nullptr)
	, m_pixelShader(nullptr)
	, m_geometryShader(nullptr)
	, m_timer(0.0f)
	, m_scale(0.0f)
	, m_life(0.0f)
{
}


/**
 * @brief デストラクタ
 */
TrajectoryParticle::~TrajectoryParticle()
{
	m_particleUtility.clear();
	m_texture.clear();

	m_pDR = nullptr;
	m_CBuffer = nullptr;
	m_inputLayout = nullptr;
	m_batch = nullptr;
	m_states = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_geometryShader = nullptr;
}


/**
 * @brief エフェクトの作成
 *
 * @param[in] DR			 デバイスリソースのポインタ
 * @param[in] CBuffer		 バッファのポインタ
 * @param[in] inputLayout	 入力レイアウトのポインタ
 * @param[in] batch			 プリミティブバッチのポインタ
 * @param[in] states		 共通ステートのポインタ
 * @param[in] texture		 テクスチャのポインタ
 * @param[in] vertexShader	 頂点シェーダーのポインタ
 * @param[in] pixelShader	 ピクセルシェーダーのポインタ
 * @param[in] geometryShader ジオメトリシェーダーのポインタ
 * @param[in] scale			 大きさ
 * @param[in] life			 寿命
 * @param[in] color			 色
 *
 * @return なし
 */
void TrajectoryParticle:: Create(DX::DeviceResources* DR,
								 ID3D11Buffer* CBuffer,
								 ID3D11InputLayout* inputLayout,
								 DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* batch,
								 DirectX::CommonStates* states,
								 ID3D11ShaderResourceView* texture,
								 ID3D11VertexShader* vertexShader,
								 ID3D11PixelShader* pixelShader,
								 ID3D11GeometryShader* geometryShader,
								 float scale, float life, DirectX::SimpleMath::Color color)			
{
	m_pDR = DR;
	ID3D11Device1* device = DR->GetD3DDevice();

	m_CBuffer = CBuffer;
	m_inputLayout = inputLayout;

	//画像の保存
	m_texture.push_back(texture);

	m_batch = batch;
	m_states = states;

	m_pixelShader = pixelShader;
	m_vertexShader = vertexShader;
	m_geometryShader = geometryShader;

	m_scale = scale;
	m_color = color;
	m_life = life;
}


/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 * @param[in] pos		  出現位置
 * @param[in] spawn		  出現させるか
 * @param[in] isRandom	  位置のランダム性
 *
 * @return なし
 */
void TrajectoryParticle::Update(float elapsedTime, SimpleMath::Vector3 pos, bool spawn, bool isRandom)
{
	//パーティクルの生成
	m_timer += elapsedTime;
	if (m_timer > 0.1f && spawn)
	{
		if (isRandom)
		{
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<> random(0.0f, XM_2PI);
			std::uniform_real_distribution<> random2(-1.0f, 1.0f);

			float angle = random(mt);
			float range = random2(mt);

			float x = cosf(angle) * range;
			float z = sinf(angle) * range;
			pos += SimpleMath::Vector3(x, 0.0f, z);
		}
	
		//座標の設定
		TrajectoryParticleUtility particleUtility{
			pos,
			SimpleMath::Vector3(m_scale,m_scale,m_scale),
			m_life,
			m_color
		};
		m_particleUtility.push_back(particleUtility);	//ビルボードしたい

		m_timer = 0.0f;
	}

	//パーティクルの更新
	for (std::list<TrajectoryParticleUtility>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
	{
		if (!(ite)->Update(elapsedTime))
		{
			//	falseが返ってきたら、消す
			ite = m_particleUtility.erase(ite);

			if (ite == m_particleUtility.end())
			{
				break;
			}
		}
	}
}


/**
 * @brief 描画処理
 *
 * @param[in] view ビュー行列
 * @param[in] proj 射影行列
 *
 * @return なし
 */
void TrajectoryParticle::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	cameraDir.Normalize();

	m_particleUtility.sort(
		[&](TrajectoryParticleUtility lhs, TrajectoryParticleUtility  rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDir.Dot(lhs.GetPosition() - m_cameraPosition) > cameraDir.Dot(rhs.GetPosition() - m_cameraPosition);
		});

	m_vertices.clear();
	for (TrajectoryParticleUtility& li : m_particleUtility)
	{
		if (cameraDir.Dot(li.GetPosition() - m_cameraPosition) < 0.0f) 	continue;

		VertexPositionColorTexture vPCT;
		vPCT.position = XMFLOAT3(li.GetPosition());
		vPCT.color = XMFLOAT4(li.GetColor());
		vPCT.textureCoordinate = DirectX::XMFLOAT2(li.GetScale().x, li.GetScale().y);

		m_vertices.push_back(vPCT);
	}

	if (m_vertices.empty()) return;

	// ビルボードを反映
	m_world = SimpleMath::Matrix::Identity;
	m_world *= m_billboard;

	ConstBuffer cbuff;

	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	context->UpdateSubresource(m_CBuffer, 0, NULL, &cbuff, 0, 0);

	ID3D11Buffer* cb[1] = { m_CBuffer };

	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_states->DepthRead(), 0);

	//	カリングはなし
	context->RSSetState(m_states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);
	context->GSSetShader(m_geometryShader, nullptr, 0);

	//	Create関数で読み込んだ画像をピクセルシェーダに登録する
	for (int i = 0; i < m_texture.size(); i++)
	{
		//	for文で一気に設定する
		context->PSSetShaderResources(i, 1, &m_texture[i]);
	}

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout);

	//	板ポリゴンを描画
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertices[0], m_vertices.size());
	m_batch->End();

	// 使ったSRVを解除
	//ID3D11ShaderResourceView* nullSRV[8] = {};
	//context->PSSetShaderResources(0, (UINT)std::min<size_t>(m_texture.size(), 8), nullSRV);

	// サンプラ解除
	//ID3D11SamplerState* nullSmp[1] = { nullptr };
	//context->PSSetSamplers(0, 1, nullSmp);

	// ブレンド/深度/ラスタ/入力レイアウトを元に戻す or 解除
	//context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	//context->OMSetDepthStencilState(nullptr, 0);
	//context->RSSetState(nullptr);
	//context->IASetInputLayout(nullptr);

	//	シェーダの登録を解除
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);

	// 定義バッファを解除
	//ID3D11Buffer* nullCB[1] = { nullptr };
	//context->VSSetConstantBuffers(0, 1, nullCB);
	//context->PSSetConstantBuffers(0, 1, nullCB);
	//context->GSSetConstantBuffers(0, 1, nullCB);
}


/**
 * @brief ビルボードの作成
 *
 * @param[in] position  位置
 * @param[in] target	カメラの注視点
 * @param[in] eye		カメラの位置
 * @param[in] up		カメラの上方向ベクトル
 *
 * @return なし
 */
void TrajectoryParticle::CreateBillboard(DirectX::SimpleMath::Vector3 position, 
										 DirectX::SimpleMath::Vector3 target, 
										 DirectX::SimpleMath::Vector3 eye,
										 DirectX::SimpleMath::Vector3 up)
{
	m_billboard =
		DirectX::SimpleMath::Matrix::CreateBillboard(position, eye, up);

	//	回転情報を設定する
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
	//	180度逆を向いているので、補正するための情報を入れる
	rot._11 = -1;
	rot._33 = -1;

	m_cameraPosition = eye;
	m_cameraTarget = target;
	m_billboard = rot * m_billboard;
}
