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

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
	std::vector<DirectX::VertexPositionColorTexture> m_vertices;

	//カメラに関する情報
	DirectX::SimpleMath::Vector3 m_cameraPosition;
	DirectX::SimpleMath::Vector3 m_cameraTarget;

	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	DirectX::SimpleMath::Matrix m_billboard;

	float m_scale;
	float m_life;
	DirectX::SimpleMath::Color m_color;

public:
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	CircleParticle();
	~CircleParticle();

	void LoadTexture(const wchar_t* path);

	void Create(DX::DeviceResources* pDR, const wchar_t* texPath, 
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
	void CreateShader();

};