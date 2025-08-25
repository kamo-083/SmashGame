//--------------------------------------------------------------------------------------
// File: CircleParticle.cpp
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "CircleParticle.h"

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


CircleParticle::CircleParticle()
	:m_pDR(nullptr)
	, m_timer(0.0f)
	,m_CBuffer(nullptr)
	,m_inputLayout(nullptr)
	,m_batch(nullptr)
	,m_states(nullptr)
	,m_texture()
	,m_vertices()
	,m_vertexShader(nullptr)
	,m_pixelShader(nullptr)
	,m_geometryShader(nullptr)
	, m_scale(0.0f)
	, m_life(0.0f)
{
}

CircleParticle::~CircleParticle()
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


void CircleParticle::Create(DX::DeviceResources* DR,
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
	m_life = life;
	m_color = color;
}


void CircleParticle::Update(float elapsedTime)
{
	m_timer += elapsedTime;

	//パーティクルの更新
	for (std::list<CircleParticleUtility>::iterator ite = m_particleUtility.begin(); ite != m_particleUtility.end(); ite++)
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

void CircleParticle::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	cameraDir.Normalize();

	m_particleUtility.sort(
		[&](CircleParticleUtility lhs, CircleParticleUtility  rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDir.Dot(lhs.GetPosition() - m_cameraPosition) > cameraDir.Dot(rhs.GetPosition() - m_cameraPosition);
		});

	m_vertices.clear();
	for (CircleParticleUtility& li : m_particleUtility)
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
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

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
	ID3D11ShaderResourceView* nullSRV[8] = {};
	context->PSSetShaderResources(0, (UINT)std::min<size_t>(m_texture.size(), 8), nullSRV);

	// サンプラ解除
	ID3D11SamplerState* nullSmp[1] = { nullptr };
	context->PSSetSamplers(0, 1, nullSmp);

	// ブレンド/深度/ラスタ/入力レイアウトを元に戻す or 解除
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
	context->IASetInputLayout(nullptr);

	//	シェーダの登録を解除
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
}

void CircleParticle::SpawnParticleEffect(DirectX::SimpleMath::Vector3 pos,
										 float range,
										 bool isRandom,
										 bool isHorizontal,
										 int num)
{
	if (m_timer < SPAWN_INTERVAL) return;

	for (int i = 0; i < num; i++)
	{
		float angle = 0.0f;
		float scale = m_scale;
		if (isRandom)
		{
			//乱数生成器
			std::random_device seed;
			std::mt19937 mt(seed());
			std::uniform_real_distribution<> random(0.0f, XM_2PI);
			std::uniform_real_distribution<> random2(-0.5f, 0.5f);

			angle = random(mt);
			scale += random2(mt);
		}
		else
		{
			angle = XM_2PI * static_cast<float>(i) / static_cast<float>(num);
		}

		SimpleMath::Vector3 transPos = pos;
		if (isHorizontal)
		{
			float x = cosf(angle) * range;
			float z = sinf(angle) * range;
			transPos += SimpleMath::Vector3(x, 0.0f, z);
		}
		else
		{
			float y = cosf(angle) * range;
			float x = sinf(angle) * range;
			transPos += SimpleMath::Vector3(x, y, 0.0f);
		}

		//座標の設定
		CircleParticleUtility particleUtility{
			transPos,
			SimpleMath::Vector3(scale,scale,scale),
			m_life,
			m_color
		};
		m_particleUtility.push_back(particleUtility);
	}

	m_timer = 0.0f;
}

void CircleParticle::CreateBillboard(DirectX::SimpleMath::Vector3 position,
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
