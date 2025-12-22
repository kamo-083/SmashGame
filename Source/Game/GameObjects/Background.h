/**
 * @file   Background.h
 *
 * @brief  背景に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/ResourceManager.h"
#include"Source/Game/Common/ShaderManager.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief 背景
 */
class Background
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// キーとパスを渡す構造体
	struct ResourcesDesc
	{
		std::string key;	// キー
		std::string path;	// ファイルパス
	};

	struct MoveDirection
	{
		static constexpr DirectX::SimpleMath::Vector2 UP = { 0,1 };
		static constexpr DirectX::SimpleMath::Vector2 DOWN = { 0,-1 };
		static constexpr DirectX::SimpleMath::Vector2 LEFT = { 1,0 };
		static constexpr DirectX::SimpleMath::Vector2 RIGHT = { -1,0 };
	};

private:
	// インプットレイアウト
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	// シェーダーに渡す定数バッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;		// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;		// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;		// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;		// 基本色
		DirectX::SimpleMath::Vector2 WindowSize;	// ウィンドウサイズ
		DirectX::SimpleMath::Vector2 MoveDir;		// 移動方向
		float Time = 0.0f;							// 経過時間
		DirectX::SimpleMath::Vector3 Dummy;
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;

	// ウィンドウサイズ
	DirectX::SimpleMath::Vector2 m_windowSize;

	// シェーダーの情報
	ShaderManager::VertexShaderEntry* m_vs;
	ShaderManager::PixelShaderEntry* m_ps;
	ShaderManager::GeometryShaderEntry* m_gs;

	// テクスチャのポインタ
	ID3D11ShaderResourceView* m_texture;

	DirectX::SimpleMath::Vector2 m_moveDirection;

	// 経過時間
	float m_timer;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Background(
		DX::DeviceResources* pDR,
		ShaderManager* pSM,
		ResourceManager* pRM,
		const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs, const ResourcesDesc& tex,
		DirectX::SimpleMath::Vector2 windowSize, DirectX::SimpleMath::Vector2 moveDir);

	// デストラクタ
	~Background();


// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context);

	// シェーダーの作成
	void LoadShader(
		DX::DeviceResources* pDR, ShaderManager* pSM,
		const ResourcesDesc& vs, const ResourcesDesc& ps, const ResourcesDesc& gs);

	// テクスチャの読み込み
	void LoadTexture(ResourceManager* pRM, const ResourcesDesc& desc);


// 取得/設定
public:

// 内部実装
private:

};
