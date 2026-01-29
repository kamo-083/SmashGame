/**
 * @file   AreaEffect.h
 *
 * @brief  エリアのエフェクトに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/Common/ShaderManager.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief エリアのエフェクト
 */
class AreaEffect
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	// コンストバッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;	// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;	// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;	// 基本色
		float Height = 0.0f;					// 高さ
		float Time = 0.0f;						// 時間
		DirectX::SimpleMath::Vector2 Dummy;		// ダミーデータ
	};

	// 初期設定の引数群
	struct SetupDesc
	{
		DirectX::SimpleMath::Vector3 position;	// 中心位置
		DirectX::SimpleMath::Vector3 length;	// 各辺の長さ
		DirectX::SimpleMath::Color color;		// 色
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// 中心位置
	DirectX::SimpleMath::Vector3 m_position;

	// 各辺の長さ
	DirectX::SimpleMath::Vector3 m_length;

	// 累計経過時間
	float m_timer;

	// バッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;

	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;

	// シェーダー
	ShaderManager::VertexShaderEntry* m_vs;		// 頂点シェーダー
	ShaderManager::PixelShaderEntry* m_ps;		// ピクセルシェーダー
	ShaderManager::GeometryShaderEntry* m_gs;	// ジオメトリシェーダー

	// 色
	DirectX::SimpleMath::Color m_color;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	AreaEffect(UserResources* pUR, const SetupDesc& desc);
	AreaEffect(UserResources* pUR);

	// デストラクタ
	~AreaEffect();


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(const RenderContext& context);

	// 終了処理
	void Finalize();

// 取得/設定
public:
	// 座標を設定
	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { m_position = pos; }

	// 各辺の長さを設定
	void SetLength(const DirectX::SimpleMath::Vector3& length) { m_length = length; }

	// 色を設定
	void SetColor(const DirectX::SimpleMath::Color& color) { m_color = color; }


// 内部実装
private:
	// シェーダーの読み込み
	void LoadShaders(ShaderManager* shaderManager, ID3D11Device* device);

};
