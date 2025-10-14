/**
 * @file   IEffectParticle.h
 *
 * @brief  IEffectParticleに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "pch.h"
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
#include "Source/Game/Effect/ParticleUtility.h"


// クラスの定義 ===============================================================
/**
 * @brief IEffectParticle
 */
class IEffectParticle
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;
		DirectX::SimpleMath::Matrix	 matView;
		DirectX::SimpleMath::Matrix	 matProj;
		DirectX::SimpleMath::Vector4 Diffuse;
	};


	// データメンバの宣言 -----------------------------------------------
protected:
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

	std::list<ParticleUtility> m_particleUtility;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	DirectX::SimpleMath::Matrix m_billboard;

	float m_scale;
	float m_life;
	DirectX::SimpleMath::Color m_color;

	bool m_isActive;	// 有効か


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	IEffectParticle();

	// デストラクタ
	~IEffectParticle() = default;


// 操作
public:
	// エフェクトを作成
	void Create(
		DX::DeviceResources* DR,
		ID3D11Buffer* CBuffer,
		ID3D11InputLayout* inputLayout,
		DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* batch,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		ID3D11VertexShader* vertexShader,
		ID3D11PixelShader* pixelShader,
		ID3D11GeometryShader* geometryShader,
		float scale, float life, DirectX::SimpleMath::Color color);

	// ビルボードを作成
	void CreateBillboard(
		DirectX::SimpleMath::Vector3 position,
		DirectX::SimpleMath::Vector3 target,
		DirectX::SimpleMath::Vector3 eye,
		DirectX::SimpleMath::Vector3 up);

	// 描画処理
	void Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	// 有効かどうか
	bool IsActive() { return m_isActive; }

	// 無効化
	void Deactivate() { m_isActive = false; }


// 取得/設定
public:

// 内部実装
private:

};
