/**
 * @file   IEffectParticle.h
 *
 * @brief  パーティクルの基底クラスに関するヘッダファイル
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
 * @brief パーティクルの基底クラス
 */
class IEffectParticle
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// シェーダーに渡す定数バッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;	// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;	// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;	// 基本色
	};


	// データメンバの宣言 -----------------------------------------------
protected:
	// デバイスリソースのポインタ
	DX::DeviceResources* m_pDR;

	// 累計経過時間
	float m_timer;

	// バッファのポインタ
	ID3D11Buffer* m_CBuffer;
	// インプットレイアウトのポインタ
	ID3D11InputLayout* m_inputLayout;

	// プリミティブバッチのポインタ
	DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* m_batch;
	// 共通ステートのポインタ
	DirectX::CommonStates* m_states;
	// テクスチャのポインタ
	std::vector<ID3D11ShaderResourceView*> m_texture;
	// 頂点の配列
	std::vector<DirectX::VertexPositionColorTexture> m_vertices;

	//カメラに関する情報
	DirectX::SimpleMath::Vector3 m_cameraPosition;	// カメラの座標
	DirectX::SimpleMath::Vector3 m_cameraTarget;	// カメラのターゲットの座標

	//シェーダー
	ID3D11VertexShader* m_vertexShader;		// 頂点シェーダー
	ID3D11PixelShader* m_pixelShader;		// ピクセルシェーダー
	ID3D11GeometryShader* m_geometryShader;	// ジオメトリシェーダー

	// パーティクルユーティリティのリスト
	std::list<ParticleUtility> m_particleUtility;

	// ワールド行列
	DirectX::SimpleMath::Matrix m_world;
	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;
	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	// ビルボード行列
	DirectX::SimpleMath::Matrix m_billboard;
	
	// 大きさ
	float m_scale;
	// 寿命
	float m_life;
	// 色
	DirectX::SimpleMath::Color m_color;

	// 有効フラグ
	bool m_isActive;


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
		DX::DeviceResources* pDR,
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
