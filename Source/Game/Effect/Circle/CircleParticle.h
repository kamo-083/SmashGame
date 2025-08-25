//--------------------------------------------------------------------------------------
// File: CircleParticle.h
//-------------------------------------------------------------------------------------

#pragma once

#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <list>

#include"CircleParticleUtility.h"

class CircleParticle
{
public:
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;
		DirectX::SimpleMath::Matrix	 matView;
		DirectX::SimpleMath::Matrix	 matProj;
		DirectX::SimpleMath::Vector4 Diffuse;
	};

private:
	std::list<CircleParticleUtility> m_particleUtility;

	DX::DeviceResources* m_pDR;

	float m_timer;

	ID3D11Buffer* m_CBuffer;
	ID3D11InputLayout* m_inputLayout;

	DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* m_batch;
	DirectX::CommonStates* m_states;
	std::vector<ID3D11ShaderResourceView*> m_texture;
	std::vector<DirectX::VertexPositionColorTexture> m_vertices;

	//カメラに関する情報
	DirectX::SimpleMath::Vector3 m_cameraPosition;
	DirectX::SimpleMath::Vector3 m_cameraTarget;

	//シェーダー
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11GeometryShader* m_geometryShader;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	DirectX::SimpleMath::Matrix m_billboard;

	float m_scale;
	float m_life;
	DirectX::SimpleMath::Color m_color;

public:
	static constexpr float SPAWN_INTERVAL = 0.15f;	// 最小生成間隔

	CircleParticle();
	~CircleParticle();

	void Create(DX::DeviceResources* DR,
				ID3D11Buffer* CBuffer,
				ID3D11InputLayout* inputLayout,
				DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* batch,
				DirectX::CommonStates* states,
				ID3D11ShaderResourceView* texture,
				ID3D11VertexShader* vertexShader,
				ID3D11PixelShader* pixelShader,
				ID3D11GeometryShader* geometryShader,
				float scale, float life, DirectX::SimpleMath::Color color);

	void Update(float elapsedTime);

	void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	void SpawnParticleEffect(DirectX::SimpleMath::Vector3 pos,
							 float range,
							 bool isRandom = true,
							 bool isHorizontal = true,
							 int num = 10);

	void CreateBillboard(DirectX::SimpleMath::Vector3 position,
						 DirectX::SimpleMath::Vector3 target,
						 DirectX::SimpleMath::Vector3 eye,
						 DirectX::SimpleMath::Vector3 up);

private:

};